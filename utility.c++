#include "utility.h++"
#include "list.h++"

#include <cstdlib>

namespace ds {
    void MallocDeleter::operator()(void* ptr) const {
        free(ptr);
    }

    unique_malloc_ptr<term_t> text_to_term(const char* text, length_t length) {
        term_t* result = reinterpret_cast<term_t*>(malloc(length));
        result->scan(text);
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<char> term_to_text(term_t* term, length_t length) {
        char* result = reinterpret_cast<char*>(malloc(length));
        *(term->print(result)) = 0;
        return unique_malloc_ptr<char>(result);
    }

    unique_malloc_ptr<term_t> ground(term_t* term, term_t* dictionary, length_t length) {
        term_t* result = reinterpret_cast<term_t*>(malloc(length));
        result->ground(term, dictionary);
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<term_t> match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, length_t length) {
        term_t* result = reinterpret_cast<term_t*>(malloc(length));
        result->match(term_1, term_2, follow_first_for_double_variable);
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<rule_t> text_to_rule(const char* text, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        result->scan(text);
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<char> rule_to_text(rule_t* rule, length_t length) {
        char* result = reinterpret_cast<char*>(malloc(length));
        *(rule->print(result)) = 0;
        return unique_malloc_ptr<char>(result);
    }

    unique_malloc_ptr<rule_t> ground(rule_t* rule, term_t* dictionary, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        result->ground(rule, dictionary);
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<rule_t> ground(rule_t* rule, rule_t* dictionary, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        result->ground(rule, dictionary);
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<rule_t> match(rule_t* rule_1, rule_t* rule_2, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        result->match(rule_1, rule_2);
        return unique_malloc_ptr<rule_t>(result);
    }
} // namespace ds
