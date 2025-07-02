#include "item.hh"
#include "list.hh"
#include "rule.hh"
#include "string.hh"
#include "term.hh"
#include "variable.hh"

#include <cstring>

namespace ds {
    namespace {
        bool term_equal(term_t* term_1, term_t* term_2) {
            if (term_1->data_size() != term_2->data_size()) {
                return false;
            }
            length_t data_size = term_1->data_size();
            for (length_t index = 0; index < data_size; ++index) {
                if (reinterpret_cast<std::byte*>(term_1)[index] != reinterpret_cast<std::byte*>(term_2)[index]) {
                    return false;
                }
            }
            return true;
        }

        void match_helper_add_to_dict(term_t* begin, term_t** mapping, term_t* term_1, term_t* term_2) {
            while (begin != *mapping) {
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
                        *mapping = nullptr;
                        return;
                    }
                }
                begin = reinterpret_cast<term_t*>(begin->tail());
            }
            // term_1没有出现过
            term_t* term = *mapping;
            list_t* pair = term->set_list()->list()->set_list_size(2);
            memcpy(pair->term(0), term_1, term_1->data_size());
            pair->update_term_size(0);
            memcpy(pair->term(1), term_2, term_2->data_size());
            pair->update_term_size(1);
            *mapping = reinterpret_cast<term_t*>(term->tail());
        }

        void match_helper_main(term_t* begin, term_t** mapping, term_t* term_1, term_t* term_2, bool follow_first_for_double_variable) {
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
                        match_helper_add_to_dict(begin, mapping, term_1, term_2);
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
                    *mapping = nullptr;
                    return;
                }
            } else if (term_1->list() && term_2->list()) {
                list_t* list_1 = term_1->list();
                list_t* list_2 = term_2->list();
                if (list_1->get_list_size() != list_2->get_list_size()) {
                    // 长度不同, 失败
                    *mapping = nullptr;
                    return;
                }
                length_t length = list_1->get_list_size();
                for (length_t index = 0; index < length; ++index) {
                    term_t* sub_term_1 = list_1->term(index);
                    term_t* sub_term_2 = list_2->term(index);
                    match_helper_main(begin, mapping, sub_term_1, sub_term_2, follow_first_for_double_variable);
                    if (*mapping == nullptr) {
                        return;
                    }
                }
            } else if (term_1->variable()) {
                // 不同类型的term, 必须要有一个term是variable才可能成功
                match_helper_add_to_dict(begin, mapping, term_1, term_2);
                return;
            } else if (term_2->variable()) {
                // 另一个term是variable, 安全了
                return;
            } else {
                // 失败
                *mapping = nullptr;
                return;
            }
        }
    } // namespace

    term_t* term_t::match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable) {
        term_t* end = this;
        match_helper_main(this, &end, term_1, term_2, follow_first_for_double_variable);
        if (end == nullptr) {
            set_null();
            return this;
        }
        length_t list_size = 0;
        term_t* begin = this;
        while (begin != end) {
            ++list_size;
            begin = reinterpret_cast<term_t*>(begin->tail());
        }
        length_t offset = sizeof(term_type_t) + sizeof(length_t) + sizeof(length_t) * list_size;
        memcpy(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(this),
            reinterpret_cast<std::byte*>(end) - reinterpret_cast<std::byte*>(this)
        );
        set_list()->list()->set_list_size(list_size);
        for (length_t index = 0; index < list_size; ++index) {
            list()->update_term_size(index);
        }
        return this;
    }

    rule_t* rule_t::match(rule_t* rule_1, rule_t* rule_2) {
        if (rule_1->premises_count() == 0 || rule_2->premises_count() != 0) {
            set_list_size(0);
            return this;
        }
        term_t* dict_1 = reinterpret_cast<term_t*>(this);
        dict_1->match(rule_1->premises(0), rule_2->only_conclusion(), false);
        if (dict_1->is_null()) {
            set_list_size(0);
            return this;
        }
        term_t* dict_2 = reinterpret_cast<term_t*>(dict_1->tail());
        dict_2->match(rule_2->only_conclusion(), rule_1->premises(0), true);
        if (dict_2->is_null()) {
            set_list_size(0);
            return this;
        }
        rule_t* candidate_1 = reinterpret_cast<rule_t*>(dict_2->tail());
        candidate_1->ground(rule_1, dict_1);
        rule_t* candidate_2 = reinterpret_cast<rule_t*>(candidate_1->tail());
        candidate_2->ground(rule_2, dict_2);
        term_t* term_1 = candidate_1->premises(0);
        term_t* term_2 = candidate_2->only_conclusion();
        if (!term_equal(term_1, term_2)) {
            set_list_size(0);
            return this;
        }
        length_t list_size = rule_1->get_list_size() - 1;
        length_t offset = sizeof(length_t) + sizeof(length_t) * list_size;
        memcpy(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(candidate_1->premises(0)->tail()),
            reinterpret_cast<std::byte*>(candidate_1->tail()) - reinterpret_cast<std::byte*>(candidate_1->premises(0)->tail())
        );
        set_list_size(list_size);
        for (length_t index = 0; index < list_size; ++index) {
            update_term_size(index);
        }
        return this;
    }
} // namespace ds
