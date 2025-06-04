#include "config.h++"
#include "item.h++"
#include "list.h++"
#include "string.h++"
#include "term.h++"
#include "utility.h++"
#include "variable.h++"

// TODO: check out of bound
// TODO: manual stack
// TODO: check exceptions

#include <iostream>

void test_1() {
    char orig_data[220];
    for (auto i = 0; i < 220; ++i) {
        orig_data[i] = 0;
    }
    std::byte* data = reinterpret_cast<std::byte*>(orig_data);

    cuds::term_t* term_1 = reinterpret_cast<cuds::term_t*>(data);
    term_1->set_variable()->variable()->name()->set_length(10)->set_string("abc");
    std::cout << term_1->variable()->name()->get_string() << "\n";

    data = term_1->tail();

    cuds::term_t* term_2 = reinterpret_cast<cuds::term_t*>(data);
    term_2->set_item()->item()->name()->set_length(3)->set_string("666666");
    std::cout << term_2->item()->name()->get_string() << "\n";

    data = term_2->tail();

    cuds::term_t* term_3 = reinterpret_cast<cuds::term_t*>(data);
    auto list = term_3->set_list()->list()->set_list_size(3);

    list->term(0)->set_variable()->variable()->name()->set_null_string("p");
    list->update_term_size(0);

    list->term(1)->set_item()->item()->name()->set_null_string("+");
    list->update_term_size(1);

    list->term(2)->set_variable()->variable()->name()->set_null_string("q");
    list->update_term_size(2);

    *reinterpret_cast<std::uint8_t*>(term_3->tail()) = 233;

    char buffer[100];
    *(term_3->print(buffer)) = 0;
    std::cout << buffer << "\n";

    data = term_3->tail();

    auto term_5 = reinterpret_cast<cuds::term_t*>(data);
    term_5->scan(buffer);
    *(term_5->print(buffer)) = 0;
    std::cout << buffer << "\n";
    data = term_5->tail();

    cuds::term_t* dict = reinterpret_cast<cuds::term_t*>(data);
    cuds::list_t* list_of_pair = dict->set_list()->list()->set_list_size(2);
    cuds::list_t* pair_1 = list_of_pair->term(0)->set_list()->list()->set_list_size(2);
    pair_1->term(0)->set_variable()->variable()->name()->set_null_string("p");
    pair_1->update_term_size(0);
    pair_1->term(1)->set_variable()->variable()->name()->set_null_string("q");
    pair_1->update_term_size(1);
    list_of_pair->update_term_size(0);
    cuds::list_t* pair_2 = list_of_pair->term(1)->set_list()->list()->set_list_size(2);
    pair_2->term(0)->set_variable()->variable()->name()->set_null_string("q");
    pair_2->update_term_size(0);
    cuds::list_t* func_x = pair_2->term(1)->set_list()->list()->set_list_size(2);
    func_x->term(0)->set_item()->item()->name()->set_null_string("func");
    func_x->update_term_size(0);
    func_x->term(1)->set_variable()->variable()->name()->set_length(5)->set_string("x");
    func_x->update_term_size(1);
    pair_2->update_term_size(1);
    list_of_pair->update_term_size(1);
    data = dict->tail();

    cuds::term_t* term_4 = reinterpret_cast<cuds::term_t*>(data);
    term_4->ground(term_3, dict);
    *(term_4->print(buffer)) = 0;
    std::cout << buffer << "\n";
    data = term_4->tail();

    auto term_6 = reinterpret_cast<cuds::term_t*>(data);
    term_6->scan(buffer);
    *(term_6->print(buffer)) = 0;
    std::cout << buffer << "\n";
    data = term_6->tail();

    std::cout << "result" << std::endl;
    for (int i = 0; i < 220; i++) {
        std::uint8_t v = reinterpret_cast<std::uint8_t*>(orig_data)[i];
        std::cout << int(v) << ' ';
    }
    std::cout << std::endl;
}

void test_2() {
    auto term = cuds::text_to_term("('a + (f 'b) + a + b + ('a 'b))", 100);
    std::cout << term->data_size() << std::endl;
    auto text = cuds::term_to_text(term.get(), 200);
    std::cout << text.get() << std::endl;

    auto dict = cuds::text_to_term("(('a (g h)) ('b 'a))", 200);

    auto result = cuds::ground(term.get(), dict.get(), 200);

    auto text_result = cuds::term_to_text(result.get(), 200);
    std::cout << text_result.get() << "\n";
}

void test_3() {
    auto term_1 = cuds::text_to_term("('p -> q)", 100);
    auto term_2 = cuds::text_to_term("(a -> 'b)", 100);
    auto dict_1 = cuds::match(term_1.get(), term_2.get(), true, 100);
    auto dict_2 = cuds::match(term_2.get(), term_1.get(), true, 100);
    auto term_1_p = cuds::ground(term_1.get(), dict_1.get(), 100);
    auto term_2_p = cuds::ground(term_2.get(), dict_2.get(), 100);
    std::cout << cuds::term_to_text(term_1_p.get(), 200).get() << std::endl;
    std::cout << cuds::term_to_text(term_2_p.get(), 200).get() << std::endl;
}

void test_4() {
    auto rule_1 = cuds::text_to_rule("('P -> 'Q) \n 'P \n ----- \n 'Q", 100);
    std::cout << cuds::rule_to_text(rule_1.get(), 200).get() << std::endl;
    auto rule_2 = cuds::text_to_rule("(('P (f x)) ('Q a))", 100);
    auto result = cuds::ground(rule_1.get(), rule_2.get(), 200);
    std::cout << cuds::rule_to_text(result.get(), 200).get() << std::endl;
    auto rule_3 = cuds::text_to_rule("(a -> b)", 100);
    auto mp_res = cuds::match(rule_1.get(), rule_3.get(), 1000);
    std::cout << cuds::rule_to_text(mp_res.get(), 200).get() << std::endl;
    auto rule_4 = cuds::text_to_rule("(a -> b -> c)", 100);
    auto fail = cuds::match(rule_1.get(), rule_4.get(), 1000);
    std::cout << fail->valid() << std::endl;
}

#include <vector>

bool check_rule_eq(cuds::rule_t* r1, cuds::rule_t* r2) {
    if (r1->data_size() != r2->data_size()) {
        return false;
    }
    auto l = r1->data_size();
    for (auto i = 0; i < l; i++) {
        if (reinterpret_cast<std::byte*>(r1)[i] != reinterpret_cast<std::byte*>(r2)[i]) {
            return false;
        }
    }
    return true;
}

template<typename V>
bool check_in(cuds::rule_t* r, V& v) {
    for (auto& i : v) {
        if (check_rule_eq(i.get(), r)) {
            return true;
        }
    }
    return false;
}

void test_5() {
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

    auto premise = cuds::text_to_rule("(! (! P))", 1000);
    auto target = cuds::text_to_rule("P", 1000);

    auto rules = std::vector<cuds::unique_malloc_ptr<cuds::rule_t>>{};
    auto facts = std::vector<cuds::unique_malloc_ptr<cuds::rule_t>>{};
    rules.emplace_back(std::move(mp));
    facts.emplace_back(std::move(axiom1));
    facts.emplace_back(std::move(axiom2));
    facts.emplace_back(std::move(axiom3));
    facts.emplace_back(std::move(premise));

    std::cout << "starting... #####################################" << std::endl;

    while (true) {
        auto rl = rules.size();
        auto fl = facts.size();
        for (auto ri = 0; ri < rl; ri++) {
            for (auto fi = 0; fi < fl; fi++) {
                auto& rule = rules[ri];
                auto& fact = facts[fi];
                auto new_result = cuds::match(rule.get(), fact.get(), 32000);
                if (!new_result->valid()) {
                    continue;
                }
                if (new_result->data_size() > 100) {
                    continue;
                }
                if (new_result->premises_count() != 0) {
                    if (!check_in(new_result.get(), rules)) {
#if 0
                        std::cout << "\n\n\nTrying...\n"
                                  << cuds::rule_to_text(rule.get(), 2000).get() << "和\n"
                                  << cuds::rule_to_text(fact.get(), 2000).get() << "\n"
                                  << std::flush;
                        std::cout << "新的结果\n";
                        std::cout << cuds::rule_to_text(new_result.get(), 2000).get() << std::endl;
#endif
                        rules.push_back(std::move(new_result));
                    }
                } else {
                    if (!check_in(new_result.get(), facts)) {
#if 0
                        std::cout << "\n\n\nTrying...\n"
                                  << cuds::rule_to_text(rule.get(), 2000).get() << "和\n"
                                  << cuds::rule_to_text(fact.get(), 2000).get() << "\n"
                                  << std::flush;
                        std::cout << "新的结果\n";
                        std::cout << cuds::rule_to_text(new_result.get(), 2000).get() << std::endl;
#endif
                        if (check_rule_eq(new_result.get(), target.get())) {
                            std::cout << "Found!!!" << std::endl;
                            return;
                        }
                        facts.push_back(std::move(new_result));
                    }
                }
            }
        }
        std::cout << rules.size() << " " << facts.size() << "\n";
        std::cout << "cycle...\n";
    }
}

#include <cstring>
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

void test_6() {
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

    auto premise = cuds::text_to_rule("(! (! P))", 1000);
    auto target = cuds::text_to_rule("P", 1000);

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

    auto buffer = cuds::unique_malloc_ptr<cuds::rule_t>(reinterpret_cast<cuds::rule_t*>(malloc(32000)));

    auto less = PointerLess();

    while (true) {
        temp_rules.clear();
        temp_facts.clear();

        for (auto& [rule, rules_cycle] : rules) {
            for (auto& [fact, facts_cycle] : facts) {
                if (rules_cycle != cycle && facts_cycle != cycle) {
                    continue;
                }
                buffer->match(rule.get(), fact.get());
                if (!buffer->valid()) {
                    continue;
                }
                if (buffer->data_size() > 100) {
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
                        std::cout << "Found\n" << cuds::rule_to_text(new_fact.get(), 32000).get() << "\n" << std::flush;
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
    }
}

#include <chrono>
#include <functional>
#include <iostream>

// 计时器函数，接受一个无参数无返回值的函数作为参数
void timer(std::function<void()> func) {
    // 获取开始时间点
    auto start = std::chrono::high_resolution_clock::now();

    // 执行被计时的函数
    func();

    // 获取结束时间点
    auto end = std::chrono::high_resolution_clock::now();

    // 计算持续时间并转换为秒
    std::chrono::duration<double> duration = end - start;

    // 输出执行时间
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
}

int main() {
    timer(test_1);
    timer(test_2);
    timer(test_3);
    timer(test_4);
    timer(test_5);
    timer(test_6);
    timer(test_6);
}
