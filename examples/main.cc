#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <set>

#include <ds/ds.hh>
#include <ds/utility.hh>

struct PointerLess {
    template<typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        if (lhs->data_size() < rhs->data_size()) {
            return true;
        }
        if (lhs->data_size() > rhs->data_size()) {
            return false;
        }
        ds::length_t data_size = lhs->data_size();
        const std::byte* lhs_data = reinterpret_cast<const std::byte*>(lhs.get());
        const std::byte* rhs_data = reinterpret_cast<const std::byte*>(rhs.get());
        for (ds::length_t index = 0; index < data_size; ++index) {
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
    int single_result_size = 10000;

    // P -> Q, P |- Q
    auto mp = ds::text_to_rule(
        "('P -> 'Q)\n"
        "'P\n"
        "----------\n"
        "'Q",
        temp_data_size
    );
    // p -> (q -> p)
    auto axiom1 = ds::text_to_rule(
        "------------------\n"
        "('p -> ('q -> 'p))\n",
        temp_data_size
    );
    // (p -> (q -> r)) -> ((p -> q) -> (p -> r))
    auto axiom2 = ds::text_to_rule(
        "--------------------------------------------------\n"
        "(('p -> ('q -> 'r)) -> (('p -> 'q) -> ('p -> 'r)))\n",
        temp_data_size
    );
    // (!p -> !q) -> (q -> p)
    auto axiom3 = ds::text_to_rule(
        "----------------------------------\n"
        "(((! 'p) -> (! 'q)) -> ('q -> 'p))\n",
        temp_data_size
    );

    auto premise = ds::text_to_rule("(! (! X))", temp_data_size);
    auto target = ds::text_to_rule("X", temp_data_size);

    std::map<std::unique_ptr<ds::rule_t>, ds::length_t, PointerLess> rules;
    std::map<std::unique_ptr<ds::rule_t>, ds::length_t, PointerLess> facts;

    std::set<std::unique_ptr<ds::rule_t>, PointerLess> temp_rules;
    std::set<std::unique_ptr<ds::rule_t>, PointerLess> temp_facts;

    ds::length_t cycle = -1;
    rules.emplace(std::move(mp), cycle);
    facts.emplace(std::move(axiom1), cycle);
    facts.emplace(std::move(axiom2), cycle);
    facts.emplace(std::move(axiom3), cycle);
    facts.emplace(std::move(premise), cycle);

    auto buffer = std::unique_ptr<ds::rule_t>(reinterpret_cast<ds::rule_t*>(operator new(single_result_size)));

    auto less = PointerLess();

    while (true) {
        temp_rules.clear();
        temp_facts.clear();

        for (auto& [rule, rules_cycle] : rules) {
            for (auto& [fact, facts_cycle] : facts) {
                if (rules_cycle != cycle && facts_cycle != cycle) {
                    continue;
                }
                buffer->match(rule.get(), fact.get(), reinterpret_cast<std::byte*>(buffer.get()) + single_result_size);
                if (!buffer->valid()) {
                    continue;
                }
                if (buffer->premises_count() != 0) {
                    // rule
                    if (rules.find(buffer) != rules.end() || temp_rules.find(buffer) != temp_rules.end()) {
                        continue;
                    }
                    auto new_rule = std::unique_ptr<ds::rule_t>(reinterpret_cast<ds::rule_t*>(operator new(buffer->data_size())));
                    memcpy(new_rule.get(), buffer.get(), buffer->data_size());
                    temp_rules.emplace(std::move(new_rule));
                } else {
                    // fact
                    if (facts.find(buffer) != facts.end() || temp_facts.find(buffer) != temp_facts.end()) {
                        continue;
                    }
                    auto new_fact = std::unique_ptr<ds::rule_t>(reinterpret_cast<ds::rule_t*>(operator new(buffer->data_size())));
                    memcpy(new_fact.get(), buffer.get(), buffer->data_size());
                    if ((!less(new_fact, target)) && (!less(target, new_fact))) {
                        printf("Found!\n");
                        printf("%s", ds::rule_to_text(new_fact.get(), temp_text_size).get());
                        return;
                    }
                    temp_facts.emplace(std::move(new_fact));
                }
            }
        }

        ++cycle;
        for (auto& rule : temp_rules) {
            auto& movable_rule = const_cast<std::unique_ptr<ds::rule_t>&>(rule);
            rules.emplace(std::move(movable_rule), cycle);
        }
        for (auto& fact : temp_facts) {
            auto& movable_fact = const_cast<std::unique_ptr<ds::rule_t>&>(fact);
            facts.emplace(std::move(movable_fact), cycle);
        }
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
