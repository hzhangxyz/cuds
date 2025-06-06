#include "config.h++"
#include "cuda_compact.h++"
#include "helper.h++"
#include "item.h++"
#include "list.h++"
#include "rule.h++"
#include "string.h++"
#include "term.h++"
#include "utility.h++"
#include "variable.h++"

#include <chrono>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <set>

// TODO: check out of bound
// TODO: manual stack
// TODO: container on CUDA
// TODO: unification algorithm need to be updated

/// @brief 验证两个rule是否相等
/// @param rule_1 第一个rule
/// @param rule_2 第二个rule
/// @return 相等则返回true, 否则是false
CUDA_HOST_DEVICE bool rule_equal(cuds::rule_t* rule_1, cuds::rule_t* rule_2) {
    if (rule_1->data_size() != rule_2->data_size()) {
        return false;
    }
    auto data_size = rule_1->data_size();
    if (cuds::memcmp(rule_1, rule_2, data_size) != 0) {
        return false;
    }
    return true;
}

enum class match_flag_t {
    null = 0,
    fail = 1,
    rule = 2,
    fact = 3,
    found = 4
};

struct match_result_t {
    /// @brief 包含了成功与否, 以及是fact还是rule等信息
    match_flag_t flag;
    /// @brief 结果rule的大小, 由于有时候也用来顺便存储指标, 所以不是length_t而是更大的size_t
    std::size_t size;
};

/// @brief 将rule和fact进行匹配的核函数
/// @param match_result 匹配的结果, 包含成功与否, 产出对象大小等信息
/// @param rule_result 产出对象
/// @param result_size_threshold 产出对象的截断大小
/// @param rule 用于匹配的rule
/// @param fact 用于匹配的fact
/// @param rules 用于去重的rules合集
/// @param rules_size 用于去重的rules合集大小
/// @param facts 用于去重的facts合集
/// @param facts_size 用于去重的facts合集大小
/// @param target 用于查询是否成功的目标对象
__device__ void match(
    match_result_t* match_result,
    cuds::rule_t* rule_result,
    cuds::length_t result_size_threshold,
    cuds::rule_t* rule,
    cuds::rule_t* fact,
    cuds::rule_t** rules,
    std::size_t rules_size,
    cuds::rule_t** facts,
    std::size_t facts_size,
    cuds::rule_t* target
) {
    rule_result->match(rule, fact);
    if (!rule_result->valid()) {
        match_result->flag = match_flag_t::fail;
        return;
    }
    if (rule_result->data_size() > result_size_threshold) {
        match_result->flag = match_flag_t::fail;
        return;
    }
    if (rule_result->premises_count() != 0) {
        // rule
        for (std::size_t rule_index = 0; rule_index < rules_size; ++rule_index) {
            cuds::rule_t* old_rule = rules[rule_index];
            if (rule_equal(old_rule, rule_result)) {
                match_result->flag = match_flag_t::fail;
                return;
            }
        }
        match_result->flag = match_flag_t::rule;
        match_result->size = rule_result->data_size();
    } else {
        // fact
        for (std::size_t fact_index = 0; fact_index < facts_size; ++fact_index) {
            cuds::rule_t* old_fact = facts[fact_index];
            if (rule_equal(old_fact, rule_result)) {
                match_result->flag = match_flag_t::fail;
                return;
            }
        }
        match_result->flag = match_flag_t::fact;
        match_result->size = rule_result->data_size();
        if (rule_equal(target, rule_result)) {
            match_result->flag = match_flag_t::found;
        }
    }
}

/// @brief 将rule和fact进行批量匹配的核函数接口
/// @param match_result_pool 匹配结果池
/// @param rule_result_pool 产出对象池
/// @param single_result_size 产出对象池中单个对象的大小
/// @param single_result_size_threshold 产出对象的截断大小
/// @param local_index_offset 单次调用的任务指标偏移
/// @param rules 用于去重的rules合集
/// @param old_old_rules_size 老rules合集的大小
/// @param old_rules_size 匹配前rules合集的大小
/// @param facts 用于去重的facts合集
/// @param old_old_facts_size 老facts合集的大小
/// @param old_facts_size 匹配前facts合集的大小
/// @param target 用于查询是否成功的目标对象
__global__ void process(
    match_result_t* match_result_pool,
    cuds::rule_t* rule_result_pool,
    cuds::length_t single_result_size,
    cuds::length_t single_result_size_threshold,
    std::size_t local_index_offset,
    cuds::rule_t** rules,
    std::size_t old_old_rules_size,
    std::size_t old_rules_size,
    cuds::rule_t** facts,
    std::size_t old_old_facts_size,
    std::size_t old_facts_size,
    cuds::rule_t* target
) {
    std::size_t local_thread_count = gridDim.x * gridDim.y * gridDim.z * blockDim.x * blockDim.y * blockDim.z;
    std::size_t local_thread_index =
        (blockIdx.z * gridDim.y * gridDim.x + blockIdx.y * gridDim.x + blockIdx.x) * (blockDim.z * blockDim.y * blockDim.x) +
        (threadIdx.z * blockDim.y * blockDim.x + threadIdx.y * blockDim.x + threadIdx.x);
    std::size_t global_job_count = old_rules_size * old_facts_size - old_old_rules_size * old_old_facts_size;
    std::size_t global_job_index = local_thread_index + local_index_offset;
    std::size_t remained_job_count = global_job_count - local_index_offset;
    std::size_t local_job_count = local_thread_count > remained_job_count ? remained_job_count : local_thread_count;

    if (global_job_index >= global_job_count) {
        return;
    }

    if (local_thread_index == 0) {
        match_result_pool[local_job_count].flag = match_flag_t::null;
    }

    std::size_t rule_index;
    std::size_t fact_index;
    if (global_job_index < (old_rules_size - old_old_rules_size) * old_facts_size) {
        rule_index = global_job_index / old_facts_size + old_old_rules_size;
        fact_index = global_job_index % old_facts_size;
    } else {
        std::size_t temp_index = global_job_index - (old_rules_size - old_old_rules_size) * old_facts_size;
        rule_index = temp_index % old_old_rules_size;
        fact_index = temp_index / old_old_rules_size + old_old_facts_size;
    }

    match_result_t* match_result = &match_result_pool[local_thread_index];
    cuds::rule_t* rule_result = cuds::with_offset(rule_result_pool, local_thread_index * single_result_size);
    match(
        match_result,
        rule_result,
        single_result_size_threshold,
        rules[rule_index],
        facts[fact_index],
        rules,
        old_rules_size,
        facts,
        old_facts_size,
        target
    );

    if (match_result->flag == match_flag_t::found) {
        match_result_pool[local_job_count].flag = match_flag_t::found;
        match_result_pool[local_job_count].size = local_thread_index;
    }
}

void run() {
    int temp_data_size = 1000;
    int temp_text_size = 1000;
    int single_result_size = 30000;
    int single_result_size_threshold = 2000;
    int cuda_stack_size = 2000;
    int max_block_per_call = 4000;
    int thread_per_block = 16;

    CHECK_CUDA_ERROR(cudaDeviceSetLimit(cudaLimitStackSize, cuda_stack_size));

    // P -> Q, P |- Q
    auto mp = cuds::text_to_rule(
        "('P -> 'Q)\n"
        "'P\n"
        "----------\n"
        "'Q",
        temp_data_size
    );
    // p -> (q -> p)
    auto axiom1 = cuds::text_to_rule(
        "------------------\n"
        "('p -> ('q -> 'p))\n",
        temp_data_size
    );
    // (p -> (q -> r)) -> ((p -> q) -> (p -> r))
    auto axiom2 = cuds::text_to_rule(
        "--------------------------------------------------\n"
        "(('p -> ('q -> 'r)) -> (('p -> 'q) -> ('p -> 'r)))\n",
        temp_data_size
    );
    // (!p -> !q) -> (q -> p)
    auto axiom3 = cuds::text_to_rule(
        "----------------------------------\n"
        "(((! 'p) -> (! 'q)) -> ('q -> 'p))\n",
        temp_data_size
    );

    auto premise = cuds::text_to_rule("(! (! X))", temp_data_size);
    auto target = cuds::text_to_rule("X", temp_data_size);

    std::vector<cuds::unique_cuda_malloc_ptr<cuds::rule_t>> rules;
    std::vector<cuds::unique_cuda_malloc_ptr<cuds::rule_t>> facts;
    rules.push_back(copy_host_to_device(mp));
    facts.push_back(copy_host_to_device(axiom1));
    facts.push_back(copy_host_to_device(axiom2));
    facts.push_back(copy_host_to_device(axiom3));
    facts.push_back(copy_host_to_device(premise));
    cuds::unique_cuda_malloc_ptr<cuds::rule_t> target_d = copy_host_to_device(target);

    double kernel_time = 0;

    std::size_t old_old_rules_size = 0;
    std::size_t old_old_facts_size = 0;
    while (true) {
        std::size_t old_rules_size = rules.size();
        std::size_t old_facts_size = facts.size();

        cuds::rule_t** rules_d;
        cuds::rule_t** facts_d;
        CHECK_CUDA_ERROR(cudaMalloc(&rules_d, old_rules_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(rules_d, rules.data(), old_rules_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));
        CHECK_CUDA_ERROR(cudaMalloc(&facts_d, old_facts_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(facts_d, facts.data(), old_facts_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));

        std::size_t global_job_count = old_rules_size * old_facts_size - old_old_rules_size * old_old_facts_size;
        std::size_t max_thread_per_call = max_block_per_call * thread_per_block;
        for (std::size_t local_index_offset = 0; local_index_offset < global_job_count; local_index_offset += max_thread_per_call) {
            printf(".");
            std::size_t remained_job_count = global_job_count - local_index_offset;
            std::size_t local_job_count = remained_job_count < max_thread_per_call ? remained_job_count : max_thread_per_call;
            std::size_t block_count = (local_job_count + thread_per_block - 1) / thread_per_block;

            match_result_t* match_result_pool_d;
            CHECK_CUDA_ERROR(cudaMalloc(&match_result_pool_d, sizeof(match_result_t) * (local_job_count + 1)));
            cuds::rule_t* rule_result_pool_d;
            CHECK_CUDA_ERROR(cudaMalloc(&rule_result_pool_d, single_result_size * local_job_count));

            auto start = std::chrono::high_resolution_clock::now();
            process<<<block_count, thread_per_block>>>(
                match_result_pool_d,
                rule_result_pool_d,
                single_result_size,
                single_result_size_threshold,
                local_index_offset,
                rules_d,
                old_old_rules_size,
                old_rules_size,
                facts_d,
                old_old_facts_size,
                old_facts_size,
                target_d.get()
            );
            CHECK_CUDA_ERROR(cudaDeviceSynchronize());
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            kernel_time += duration.count();

            match_result_t* match_result_pool_h = reinterpret_cast<match_result_t*>(malloc(sizeof(match_result_t) * (local_job_count + 1)));
            CHECK_CUDA_ERROR(
                cudaMemcpy(match_result_pool_h, match_result_pool_d, sizeof(match_result_t) * (local_job_count + 1), cudaMemcpyDeviceToHost)
            );

            if (match_result_pool_h[local_job_count].flag == match_flag_t::found) {
                std::size_t job_index = match_result_pool_h[local_job_count].size;
                match_result_t* match_result_h = &match_result_pool_h[job_index];
                cuds::rule_t* result_n = reinterpret_cast<cuds::rule_t*>(malloc(match_result_h->size));
                CHECK_CUDA_ERROR(cudaMemcpy(
                    result_n,
                    cuds::with_offset(rule_result_pool_d, single_result_size * job_index),
                    match_result_h->size,
                    cudaMemcpyDeviceToHost
                ));
                char* text_n = reinterpret_cast<char*>(malloc(temp_text_size));
                *(result_n->print(text_n)) = 0;
                printf("Found!\n");
                printf("%s", text_n);
                printf("Last job count: %d\n", global_job_count);
                printf("Kernel time: %lf seconds\n", kernel_time);
                free(text_n);
                free(result_n);
                free(match_result_pool_h);
                CHECK_CUDA_ERROR(cudaFree(rule_result_pool_d));
                CHECK_CUDA_ERROR(cudaFree(match_result_pool_d));
                CHECK_CUDA_ERROR(cudaFree(rules_d));
                CHECK_CUDA_ERROR(cudaFree(facts_d));
                return;
            }

            for (int job_index = 0; job_index < local_job_count; ++job_index) {
                match_result_t* match_result_h = &match_result_pool_h[job_index];
                if (match_result_h->flag == match_flag_t::fact || match_result_h->flag == match_flag_t::rule) {
                    cuds::rule_t* result_n;
                    CHECK_CUDA_ERROR(cudaMalloc(&result_n, match_result_h->size));
                    CHECK_CUDA_ERROR(cudaMemcpy(
                        result_n,
                        cuds::with_offset(rule_result_pool_d, single_result_size * job_index),
                        match_result_h->size,
                        cudaMemcpyDeviceToDevice
                    ));

                    if (match_result_h->flag == match_flag_t::rule) {
                        // rule
                        rules.emplace_back(result_n);
                    } else {
                        // fact
                        facts.emplace_back(result_n);
                    }
                }
            }

            free(match_result_pool_h);
            CHECK_CUDA_ERROR(cudaFree(rule_result_pool_d));
            CHECK_CUDA_ERROR(cudaFree(match_result_pool_d));
        }

        CHECK_CUDA_ERROR(cudaFree(rules_d));
        CHECK_CUDA_ERROR(cudaFree(facts_d));

        old_old_rules_size = old_rules_size;
        old_old_facts_size = old_facts_size;

        printf("\n");
    }
}

void timer(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds\n" << std::flush;
}

int main() {
    for (auto i = 0; i < 10; ++i) {
        timer(run);
    }
}
