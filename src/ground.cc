#include <cstring>

#include <ds/item.hh>
#include <ds/list.hh>
#include <ds/rule.hh>
#include <ds/term.hh>
#include <ds/variable.hh>

namespace ds {
    term_t* term_t::ground(term_t* term, term_t* dictionary, std::byte* check_tail) {
        if (term->get_type() == term_type_t::variable) {
            char* this_string = term->variable()->name()->get_string();
            list_t* list = dictionary->list();
            for (length_t index = 0; index < list->get_list_size(); ++index) {
                list_t* pair = list->term(index)->list();
                variable_t* key = pair->term(0)->variable();
                term_t* value = pair->term(1);
                char* key_string = key->name()->get_string();
                if (strcmp(this_string, key_string) == 0) {
                    if (check_tail) {
                        if (check_tail < reinterpret_cast<std::byte*>(this) + value->data_size()) {
                            return nullptr;
                        }
                    }
                    memcpy(this, value, value->data_size());
                    return this;
                }
            }
            if (check_tail) {
                if (check_tail < reinterpret_cast<std::byte*>(this) + term->data_size()) {
                    return nullptr;
                }
            }
            memcpy(this, term, term->data_size());
            return this;
        } else if (term->get_type() == term_type_t::item) {
            if (check_tail) {
                if (check_tail < reinterpret_cast<std::byte*>(this) + term->data_size()) {
                    return nullptr;
                }
            }
            memcpy(this, term, term->data_size());
            return this;
        } else if (term->get_type() == term_type_t::list) {
            list_t* src = term->list();
            if (set_list(check_tail) == nullptr) {
                return nullptr;
            }
            list_t* dst = list();
            if (dst->set_list_size(src->get_list_size(), check_tail) == nullptr) {
                return nullptr;
            }
            for (length_t index = 0; index < dst->get_list_size(); ++index) {
                if (dst->term(index)->ground(src->term(index), dictionary, check_tail) == nullptr) {
                    return nullptr;
                }
                dst->update_term_size(index);
            }
            return this;
        }
        return nullptr;
    }

    rule_t* rule_t::ground(rule_t* rule, term_t* dictionary, std::byte* check_tail) {
        list_t* dst = this;
        list_t* src = rule;
        if (dst->set_list_size(src->get_list_size(), check_tail) == nullptr) {
            return nullptr;
        }
        for (length_t index = 0; index < dst->get_list_size(); ++index) {
            if (dst->term(index)->ground(src->term(index), dictionary, check_tail) == nullptr) {
                return nullptr;
            }
            dst->update_term_size(index);
        }
        return this;
    }

    rule_t* rule_t::ground(rule_t* rule, rule_t* dictionary, std::byte* check_tail) {
        return ground(rule, dictionary->only_conclusion(), check_tail);
    }
} // namespace ds
