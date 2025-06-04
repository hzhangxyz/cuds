#include "cuda_compact.h++"
#include "item.h++"
#include "list.h++"
#include "rule.h++"
#include "term.h++"
#include "variable.h++"

namespace cuds {
    CUDA_HOST_DEVICE term_t* term_t::ground(term_t* term, term_t* dictionary) {
        if (term->get_type() == term_type_t::variable) {
            char* this_string = term->variable()->name()->get_string();
            list_t* list = dictionary->list();
            for (length_t index = 0; index < list->get_list_size(); ++index) {
                list_t* pair = list->term(index)->list();
                variable_t* key = pair->term(0)->variable();
                term_t* value = pair->term(1);
                char* key_string = key->name()->get_string();
                if (strcmp(this_string, key_string) == 0) {
                    memcpy(this, value, value->data_size());
                    return this;
                }
            }
            memcpy(this, term, term->data_size());
            return this;
        } else if (term->get_type() == term_type_t::item) {
            memcpy(this, term, term->data_size());
            return this;
        } else if (term->get_type() == term_type_t::list) {
            list_t* src = term->list();
            list_t* dst = set_list()->list();
            dst->set_list_size(src->get_list_size());
            for (length_t index = 0; index < dst->get_list_size(); ++index) {
                dst->term(index)->ground(src->term(index), dictionary);
                dst->update_term_size(index);
            }
            return this;
        }
        set_null();
        return this;
    }

    CUDA_HOST_DEVICE rule_t* rule_t::ground(rule_t* rule, term_t* dictionary) {
        list_t* dst = this;
        list_t* src = rule;
        dst->set_list_size(src->get_list_size());
        for (length_t index = 0; index < dst->get_list_size(); ++index) {
            dst->term(index)->ground(src->term(index), dictionary);
            dst->update_term_size(index);
        }
        return this;
    }

    CUDA_HOST_DEVICE rule_t* rule_t::ground(rule_t* rule, rule_t* dictionary) {
        ground(rule, dictionary->only_conclusion());
        return this;
    }
} // namespace cuds
