#include <cstring>

#include <ds/item.hh>
#include <ds/list.hh>
#include <ds/rule.hh>
#include <ds/string.hh>
#include <ds/term.hh>
#include <ds/variable.hh>

namespace ds {
    namespace {
        bool term_equal(term_t* term_1, term_t* term_2) {
            if (term_1->data_size() != term_2->data_size()) {
                return false;
            }
            length_t data_size = term_1->data_size();
            if (memcmp(term_1, term_2, data_size) != 0) {
                return false;
            }
            return true;
        }

        void match_helper_add_to_dict(term_t* begin, term_t*& end, term_t* term_1, term_t* term_2, std::byte* check_tail) {
            // 将term_1 匹配到 term_2 添加到字典中
            // 字典以begin为开始，end为结尾
            // 我们通过更新end来完成字典的添加，如果匹配失败则将end设置为nullptr
            while (begin != end) {
                list_t* pair = begin->list();
                term_t* key = pair->term(0);
                term_t* value = pair->term(1);
                if (strcmp(key->variable()->name()->get_string(), term_1->variable()->name()->get_string()) == 0) {
                    // key == term_1, 需要判断value是否等于term_2
                    if (term_equal(value, term_2)) {
                        // 没问题, 直接跳过
                        return;
                    } else {
                        // 失败
                        end = nullptr;
                        return;
                    }
                }
                begin = reinterpret_cast<term_t*>(begin->tail());
            }
            // term_1没有出现过
            term_t* term = end;
            if (check_tail != nullptr) {
                if (check_tail < reinterpret_cast<std::byte*>(term) + sizeof(term_type_t) + sizeof(length_t) * 3) {
                    end = nullptr;
                    return;
                }
            }
            list_t* pair = term->set_list(nullptr)->list()->set_list_size(2, nullptr);
            if (check_tail != nullptr) {
                if (check_tail < reinterpret_cast<std::byte*>(pair->term(0)) + term_1->data_size()) {
                    end = nullptr;
                    return;
                }
            }
            memcpy(pair->term(0), term_1, term_1->data_size());
            pair->update_term_size(0);
            if (check_tail != nullptr) {
                if (check_tail < reinterpret_cast<std::byte*>(pair->term(1)) + term_2->data_size()) {
                    end = nullptr;
                    return;
                }
            }
            memcpy(pair->term(1), term_2, term_2->data_size());
            pair->update_term_size(1);
            end = reinterpret_cast<term_t*>(term->tail());
        }

        void
        match_helper_main(term_t* begin, term_t*& end, term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, std::byte* check_tail) {
            if (term_1->variable() && term_2->variable()) {
                // 两个都是variable
                if (strcmp(term_1->variable()->name()->get_string(), term_2->variable()->name()->get_string()) == 0) {
                    // 如果是相同的variable, 不做改变
                    return;
                } else {
                    // 如果是不同的variable, 将一个变成另一个
                    if (follow_first_for_double_variable) {
                        return;
                    } else {
                        match_helper_add_to_dict(begin, end, term_1, term_2, check_tail);
                        return;
                    }
                }
            } else if (term_1->item() && term_2->item()) {
                // 两个都是item
                if (strcmp(term_1->item()->name()->get_string(), term_2->item()->name()->get_string()) == 0) {
                    // 如果相同则没事
                    return;
                } else {
                    // 如果不同则失败
                    end = nullptr;
                    return;
                }
            } else if (term_1->list() && term_2->list()) {
                list_t* list_1 = term_1->list();
                list_t* list_2 = term_2->list();
                if (list_1->get_list_size() != list_2->get_list_size()) {
                    // 长度不同, 失败
                    end = nullptr;
                    return;
                }
                length_t length = list_1->get_list_size();
                for (length_t index = 0; index < length; ++index) {
                    term_t* sub_term_1 = list_1->term(index);
                    term_t* sub_term_2 = list_2->term(index);
                    match_helper_main(begin, end, sub_term_1, sub_term_2, follow_first_for_double_variable, check_tail);
                    if (end == nullptr) {
                        return;
                    }
                }
                return;
            } else if (term_1->variable()) {
                // 不同类型的term, 必须要有一个term是variable才可能成功
                match_helper_add_to_dict(begin, end, term_1, term_2, check_tail);
                return;
            } else if (term_2->variable()) {
                // 另一个term是variable, 安全了
                return;
            } else {
                // 失败
                end = nullptr;
                return;
            }
        }
    } // namespace

    term_t* term_t::match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, std::byte* check_tail) {
        // 检查是否能存下非法信息，存不下直接返回nullptr
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(this) + sizeof(term_type_t)) {
                return nullptr;
            }
        }
        // 在此之后，所有非法结果在返回前都会调用set_null(nullptr)
        // 将自己作为暂时的buffer用于存储一群pair形式的term
        term_t* end = this;
        match_helper_main(this, end, term_1, term_2, follow_first_for_double_variable, check_tail);
        if (end == nullptr) {
            set_null(nullptr);
            return nullptr;
        }
        // 数一下这些pair的数目
        length_t list_size = 0;
        term_t* begin = this;
        while (begin != end) {
            ++list_size;
            begin = reinterpret_cast<term_t*>(begin->tail());
        }
        // 计算offset将他们移动到后面，并在前面添加元信息
        length_t offset = sizeof(term_type_t) + sizeof(length_t) + sizeof(length_t) * list_size;
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(end) + offset) {
                set_null(nullptr);
                return nullptr;
            }
        }
        memmove(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(this),
            reinterpret_cast<std::byte*>(end) - reinterpret_cast<std::byte*>(this)
        );
        // 前面的元信息不需要检查尾指针
        set_list(nullptr)->list()->set_list_size(list_size, nullptr);
        for (length_t index = 0; index < list_size; ++index) {
            list()->update_term_size(index);
        }
        return this;
    }

    rule_t* rule_t::match(rule_t* rule_1, rule_t* rule_2, std::byte* check_tail) {
        // 检查是否能存下非法信息，存不下直接返回nullptr
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(this) + sizeof(length_t)) {
                return nullptr;
            }
        }
        // 在此之后，所有非法结果在返回前都会调用set_null(nullptr)
        // 前者部署真rule，后者不是真fact，则直接报告非法
        if (rule_1->premises_count() == 0 || rule_2->premises_count() != 0) {
            set_null(nullptr);
            return nullptr;
        }
        // 拿自己当buffer存两个dict
        term_t* dict_1 = reinterpret_cast<term_t*>(this);
        if (dict_1->match(rule_1->premises(0), rule_2->only_conclusion(), false, check_tail) == nullptr) {
            set_null(nullptr);
            return nullptr;
        }
        term_t* dict_2 = reinterpret_cast<term_t*>(dict_1->tail());
        if (dict_2->match(rule_2->only_conclusion(), rule_1->premises(0), true, check_tail) == nullptr) {
            set_null(nullptr);
            return nullptr;
        }
        // 那自己继续当buffer存ground的结果
        rule_t* candidate_1 = reinterpret_cast<rule_t*>(dict_2->tail());
        if (candidate_1->ground(rule_1, dict_1, check_tail) == nullptr) {
            set_null(nullptr);
            return nullptr;
        }
        rule_t* candidate_2 = reinterpret_cast<rule_t*>(candidate_1->tail());
        if (candidate_2->ground(rule_2, dict_2, check_tail) == nullptr) {
            set_null(nullptr);
            return nullptr;
        }
        // 检查两个candidate的对应位置是否相同
        term_t* term_1 = candidate_1->premises(0);
        term_t* term_2 = candidate_2->only_conclusion();
        if (!term_equal(term_1, term_2)) {
            set_null(nullptr);
            return nullptr;
        }
        // 进行向前位移
        length_t list_size = rule_1->get_list_size() - 1;
        length_t offset = sizeof(length_t) + sizeof(length_t) * list_size;
        memmove(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(candidate_1->premises(0)->tail()),
            reinterpret_cast<std::byte*>(candidate_1->tail()) - reinterpret_cast<std::byte*>(candidate_1->premises(0)->tail())
        );
        set_list_size(list_size, nullptr);
        for (length_t index = 0; index < list_size; ++index) {
            update_term_size(index);
        }
        return this;
    }
} // namespace ds
