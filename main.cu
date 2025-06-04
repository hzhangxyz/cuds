#include "config.h++"
#include "cuda_compact.h++"
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

__global__ void match(
    match_flag_t* flag,
    cuds::length_t* size,
    cuds::rule_t* result,
    cuds::length_t threshold,
    cuds::rule_t* rule_1,
    cuds::rule_t* rule_2,
    cuds::rule_t** rules,
    cuds::length_t rules_size,
    cuds::rule_t** facts,
    cuds::length_t facts_size,
    cuds::rule_t* target
) {
    result->match(rule_1, rule_2);
    if (!result->valid()) {
        *flag = match_flag_t::fail;
        return;
    }
    if (result->data_size() > threshold) {
        *flag = match_flag_t::fail;
        return;
    }
    if (result->premises_count() != 0) {
        // rule
        for (cuds::length_t rule_index = 0; rule_index < rules_size; ++rule_index) {
            cuds::rule_t* old_rule = rules[rule_index];
            if (rule_equal(old_rule, result)) {
                *flag = match_flag_t::fail;
                return;
            }
        }
        *flag = match_flag_t::rule;
        *size = result->data_size();
    } else {
        // fact
        for (cuds::length_t fact_index = 0; fact_index < facts_size; ++fact_index) {
            cuds::rule_t* old_fact = facts[fact_index];
            if (rule_equal(old_fact, result)) {
                *flag = match_flag_t::fail;
                return;
            }
        }
        *flag = match_flag_t::fact;
        *size = result->data_size();
        if (rule_equal(target, result)) {
            *flag = match_flag_t::found;
        }
    }
}

void run() {
    cudaDeviceSetLimit(cudaLimitStackSize, 2000);

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

    cuds::length_t old_old_rules_size = 0;
    cuds::length_t old_old_facts_size = 0;
    while (true) {
        cuds::length_t old_rules_size = rules.size();
        cuds::length_t old_facts_size = facts.size();

        cuds::rule_t** device_rules;
        cuds::rule_t** device_facts;
        CHECK_CUDA_ERROR(cudaMalloc(&device_rules, old_rules_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(device_rules, rules.data(), old_rules_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));
        CHECK_CUDA_ERROR(cudaMalloc(&device_facts, old_facts_size * sizeof(cuds::rule_t*)));
        CHECK_CUDA_ERROR(cudaMemcpy(device_facts, facts.data(), old_facts_size * sizeof(cuds::rule_t*), cudaMemcpyHostToDevice));

        for (cuds::length_t rule_index = 0; rule_index < old_rules_size; ++rule_index) {
            for (cuds::length_t fact_index = 0; fact_index < old_facts_size; ++fact_index) {
                if (rule_index < old_old_rules_size && fact_index < old_old_facts_size) {
                    continue;
                }

                cuds::rule_t* result_d;
                CHECK_CUDA_ERROR(cudaMalloc(&result_d, 32000));
                match_flag_t* flag_d;
                CHECK_CUDA_ERROR(cudaMalloc(&flag_d, sizeof(match_flag_t)));
                cuds::length_t* size_d;
                CHECK_CUDA_ERROR(cudaMalloc(&size_d, sizeof(cuds::length_t)));

                match<<<1, 1>>>(
                    flag_d,
                    size_d,
                    result_d,
                    80,
                    rules[rule_index].get(),
                    facts[fact_index].get(),
                    device_rules,
                    old_rules_size,
                    device_facts,
                    old_facts_size,
                    target_d.get()
                );

                cuds::length_t size_h;
                cudaMemcpy(&size_h, size_d, sizeof(cuds::length_t), cudaMemcpyDeviceToHost);
                match_flag_t flag_h;
                cudaMemcpy(&flag_h, flag_d, sizeof(match_flag_t), cudaMemcpyDeviceToHost);

                CHECK_CUDA_ERROR(cudaFree(size_d));
                CHECK_CUDA_ERROR(cudaFree(flag_d));
                if (flag_h == match_flag_t::found) {
                    printf("Found!\n");
                    CHECK_CUDA_ERROR(cudaFree(result_d));
                    return;
                }
                if (flag_h == match_flag_t::fact || flag_h == match_flag_t::rule) {
                    cuds::rule_t* result_n;
                    CHECK_CUDA_ERROR(cudaMalloc(&result_n, size_h));
                    CHECK_CUDA_ERROR(cudaMemcpy(result_n, result_d, size_h, cudaMemcpyDeviceToDevice));

                    if (flag_h == match_flag_t::rule) {
                        // rule
                        rules.emplace_back(result_n);
                    } else {
                        // fact
                        facts.emplace_back(result_n);
                    }
                }
                CHECK_CUDA_ERROR(cudaFree(result_d));
            }
        }

        CHECK_CUDA_ERROR(cudaFree(device_rules));
        CHECK_CUDA_ERROR(cudaFree(device_facts));

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
    std::cout << "Run again...\n" << std::flush;
    timer(run);
}
