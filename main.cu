#include "config.h++"
#include "cuda_compact.h++"
#include "helper.h++"
#include "item.h++"
#include "list.h++"
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
    match_flag_t flag;
    cuds::length_t size;
};

__device__ void match(
    match_result_t* match_result,
    cuds::rule_t* rule_result,
    cuds::length_t result_size_threshold,
    cuds::rule_t* rule,
    cuds::rule_t* fact,
    cuds::rule_t** rules,
    cuds::length_t rules_size,
    cuds::rule_t** facts,
    cuds::length_t facts_size,
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
        for (cuds::length_t rule_index = 0; rule_index < rules_size; ++rule_index) {
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
        for (cuds::length_t fact_index = 0; fact_index < facts_size; ++fact_index) {
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

__global__ void process(
    match_result_t* match_result_pool,
    cuds::rule_t* rule_result_pool,
    cuds::length_t single_result_size,
    cuds::length_t single_result_size_threshold,
    cuds::rule_t** rules,
    cuds::length_t old_old_rules_size,
    cuds::length_t old_rules_size,
    cuds::rule_t** facts,
    cuds::length_t old_old_facts_size,
    cuds::length_t old_facts_size,
    cuds::rule_t* target
) {
    int thread_count = int(old_rules_size) * int(old_facts_size) - int(old_old_rules_size) * int(old_old_facts_size);
    int thread_index = (blockIdx.z * gridDim.y * gridDim.x + blockIdx.y * gridDim.x + blockIdx.x) * (blockDim.z * blockDim.y * blockDim.x) +
                       (threadIdx.z * blockDim.y * blockDim.x + threadIdx.y * blockDim.x + threadIdx.x);

    if (thread_index >= thread_count) {
        return;
    }

    int rule_index;
    int fact_index;
    if (thread_index < (old_rules_size - old_old_rules_size) * old_facts_size) {
        rule_index = thread_index / old_facts_size + old_old_rules_size;
        fact_index = thread_index % old_facts_size;
    } else {
        int temp_index = thread_index - (old_rules_size - old_old_rules_size) * old_facts_size;
        rule_index = temp_index % old_old_rules_size;
        fact_index = temp_index / old_old_rules_size + old_old_facts_size;
    }

    if (thread_index == 0) {
        match_result_pool[thread_count].flag = match_flag_t::null;
    }

    match_result_t* match_result = &match_result_pool[thread_index];
    cuds::rule_t* rule_result = cuds::with_offset(rule_result_pool, thread_index * single_result_size);
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
        match_result_pool[thread_count].flag = match_flag_t::found;
    }
}

void run() {
    int single_result_size = 32000;
    int single_result_size_threshold = 80;
    int cuda_stack_size = 2000;
    int thread_per_block = 32;

    CHECK_CUDA_ERROR(cudaDeviceSetLimit(cudaLimitStackSize, cuda_stack_size));

    // P -> Q, P |- Q
    auto mp = cuds::text_to_rule(
        "('P -> 'Q)\n"
        "'P\n"
        "----------\n"
        "'Q",
        1000
    );
    // p -> (q -> p)
    auto axiom1 = cuds::text_to_rule(
        "------------------\n"
        "('p -> ('q -> 'p))\n",
        1000
    );
    // (p -> (q -> r)) -> ((p -> q) -> (p -> r))
    auto axiom2 = cuds::text_to_rule(
        "--------------------------------------------------\n"
        "(('p -> ('q -> 'r)) -> (('p -> 'q) -> ('p -> 'r)))\n",
        1000
    );
    // (!p -> !q) -> (q -> p)
    auto axiom3 = cuds::text_to_rule(
        "----------------------------------\n"
        "(((! 'p) -> (! 'q)) -> ('q -> 'p))\n",
        1000
    );

    auto premise = cuds::text_to_rule("(! (! X))", 1000);
    auto target = cuds::text_to_rule("X", 1000);

    std::vector<cuds::unique_cuda_malloc_ptr<cuds::rule_t>> rules;
    std::vector<cuds::unique_cuda_malloc_ptr<cuds::rule_t>> facts;
    rules.push_back(copy_host_to_device(mp));
    facts.push_back(copy_host_to_device(axiom1));
    facts.push_back(copy_host_to_device(axiom2));
    facts.push_back(copy_host_to_device(axiom3));
    facts.push_back(copy_host_to_device(premise));
    cuds::unique_cuda_malloc_ptr<cuds::rule_t> target_d = copy_host_to_device(target);

    double kernel_time;

    cuds::length_t old_old_rules_size = 0;
    cuds::length_t old_old_facts_size = 0;
    while (true) {
        cuds::length_t old_rules_size = rules.size();
        cuds::length_t old_facts_size = facts.size();

        cuds::rule_t** rules_d;
        cuds::rule_t** facts_d;
        CHECK_CUDA_ERROR(cudaMalloc(&rules_d, old_rules_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(rules_d, rules.data(), old_rules_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));
        CHECK_CUDA_ERROR(cudaMalloc(&facts_d, old_facts_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(facts_d, facts.data(), old_facts_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));

        int thread_count = int(old_rules_size) * int(old_facts_size) - int(old_old_rules_size) * int(old_old_facts_size);

        match_result_t* match_result_pool_d;
        CHECK_CUDA_ERROR(cudaMalloc(&match_result_pool_d, sizeof(match_result_t) * (thread_count + 1)));
        cuds::rule_t* result_pool_d;
        CHECK_CUDA_ERROR(cudaMalloc(&result_pool_d, single_result_size * thread_count));

        auto start = std::chrono::high_resolution_clock::now();
        process<<<(thread_count + thread_per_block - 1) / thread_per_block, thread_per_block>>>(
            match_result_pool_d,
            result_pool_d,
            single_result_size,
            single_result_size_threshold,
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

        match_result_t* match_result_pool_h = reinterpret_cast<match_result_t*>(malloc(sizeof(match_result_t) * (thread_count + 1)));
        CHECK_CUDA_ERROR(cudaMemcpy(match_result_pool_h, match_result_pool_d, sizeof(match_result_t) * (thread_count + 1), cudaMemcpyDeviceToHost));

        if (match_result_pool_h[thread_count].flag == match_flag_t::found) {
            printf("Found!\n");
            printf("Kernel time: %lf seconds\n", kernel_time);
            printf("Last thread count: %d\n", thread_count);
            return;
        }

        for (int thread_index = 0; thread_index < thread_count; ++thread_index) {
            match_result_t* match_result_h = &match_result_pool_h[thread_index];
            if (match_result_h->flag == match_flag_t::fact || match_result_h->flag == match_flag_t::rule) {
                cuds::rule_t* result_n;
                CHECK_CUDA_ERROR(cudaMalloc(&result_n, match_result_h->size));
                CHECK_CUDA_ERROR(cudaMemcpy(
                    result_n,
                    cuds::with_offset(result_pool_d, single_result_size * thread_index),
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

        CHECK_CUDA_ERROR(cudaFree(match_result_pool_d));

        CHECK_CUDA_ERROR(cudaFree(rules_d));
        CHECK_CUDA_ERROR(cudaFree(facts_d));

        old_old_rules_size = old_rules_size;
        old_old_facts_size = old_facts_size;
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
    timer(run);
    timer(run);
    timer(run);
    timer(run);
}
