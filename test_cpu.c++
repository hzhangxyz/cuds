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
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <set>

struct PointerLess {
    template<typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        if (lhs->data_size() < rhs->data_size()) {
            return true;
        }
        if (lhs->data_size() > rhs->data_size()) {
            return false;
        }
        cuds::length_t data_size = lhs->data_size();
        const std::byte* lhs_data = reinterpret_cast<const std::byte*>(lhs.get());
        const std::byte* rhs_data = reinterpret_cast<const std::byte*>(rhs.get());
        for (cuds::length_t index = 0; index < data_size; ++index) {
            if (lhs_data[index] < rhs_data[index]) {
                return true;
            }
            if (lhs_data[index] > rhs_data[index]) {
                return false;
            }
        }
        return false;
    }
};

void run() {
    int temp_data_size = 1000;
    int temp_text_size = 1000;
    int single_result_size = 30000;
    int single_result_size_threshold = 2000;

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

    std::map<cuds::unique_malloc_ptr<cuds::rule_t>, cuds::length_t, PointerLess> rules;
    std::map<cuds::unique_malloc_ptr<cuds::rule_t>, cuds::length_t, PointerLess> facts;

    std::set<cuds::unique_malloc_ptr<cuds::rule_t>, PointerLess> temp_rules;
    std::set<cuds::unique_malloc_ptr<cuds::rule_t>, PointerLess> temp_facts;

    cuds::length_t cycle = -1;
    rules.emplace(std::move(mp), cycle);
    facts.emplace(std::move(axiom1), cycle);
    facts.emplace(std::move(axiom2), cycle);
    facts.emplace(std::move(axiom3), cycle);
    facts.emplace(std::move(premise), cycle);

    auto buffer = cuds::unique_malloc_ptr<cuds::rule_t>(reinterpret_cast<cuds::rule_t*>(malloc(single_result_size)));

    auto less = PointerLess();

    while (true) {
        temp_rules.clear();
        temp_facts.clear();

        printf(".");
        for (auto& [rule, rules_cycle] : rules) {
            for (auto& [fact, facts_cycle] : facts) {
                if (rules_cycle != cycle && facts_cycle != cycle) {
                    continue;
                }
                buffer->match(rule.get(), fact.get());
                if (!buffer->valid()) {
                    continue;
                }
                if (buffer->data_size() > single_result_size_threshold) {
                    continue;
                }
                if (buffer->premises_count() != 0) {
                    // rule
                    if (rules.find(buffer) != rules.end() || temp_rules.find(buffer) != temp_rules.end()) {
                        continue;
                    }
                    auto new_rule = cuds::unique_malloc_ptr<cuds::rule_t>(reinterpret_cast<cuds::rule_t*>(malloc(buffer->data_size())));
                    memcpy(new_rule.get(), buffer.get(), buffer->data_size());
                    temp_rules.emplace(std::move(new_rule));
                } else {
                    // fact
                    if (facts.find(buffer) != facts.end() || temp_facts.find(buffer) != temp_facts.end()) {
                        continue;
                    }
                    auto new_fact = cuds::unique_malloc_ptr<cuds::rule_t>(reinterpret_cast<cuds::rule_t*>(malloc(buffer->data_size())));
                    memcpy(new_fact.get(), buffer.get(), buffer->data_size());
                    if ((!less(new_fact, target)) && (!less(target, new_fact))) {
                        printf("Found!\n");
                        printf("%s", cuds::rule_to_text(new_fact.get(), temp_text_size).get());
                        return;
                    }
                    temp_facts.emplace(std::move(new_fact));
                }
            }
        }

        ++cycle;
        for (auto& rule : temp_rules) {
            auto& movable_rule = const_cast<cuds::unique_malloc_ptr<cuds::rule_t>&>(rule);
            rules.emplace(std::move(movable_rule), cycle);
        }
        for (auto& fact : temp_facts) {
            auto& movable_fact = const_cast<cuds::unique_malloc_ptr<cuds::rule_t>&>(fact);
            facts.emplace(std::move(movable_fact), cycle);
        }

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
