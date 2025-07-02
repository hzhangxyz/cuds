#include "utility.hh"
#include "list.hh"

#include <cstdlib>

namespace ds {
    void MallocDeleter::operator()(void* ptr) const {
        free(ptr);
    }

    unique_malloc_ptr<term_t> text_to_term(const char* text, length_t length) {
        auto result = reinterpret_cast<term_t*>(malloc(length));
        auto scan_result = result->scan(text, reinterpret_cast<std::byte*>(result) + length);
        if (scan_result == nullptr) {
            free(result);
            return unique_malloc_ptr<term_t>(nullptr);
        }
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<char> term_to_text(term_t* term, length_t length) {
        auto result = reinterpret_cast<char*>(malloc(length));
        auto print_result = term->print(result, reinterpret_cast<char*>(result) + length);
        if (print_result == nullptr || print_result - result == length - 1) {
            free(result);
            return unique_malloc_ptr<char>(nullptr);
        }
        *print_result = 0;
        return unique_malloc_ptr<char>(result);
    }

    unique_malloc_ptr<term_t> ground(term_t* term, term_t* dictionary, length_t length) {
        auto result = reinterpret_cast<term_t*>(malloc(length));
        if (result->ground(term, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            free(result);
            return unique_malloc_ptr<term_t>(nullptr);
        }
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<term_t> match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, length_t length) {
        auto result = reinterpret_cast<term_t*>(malloc(length));
        if (result->match(term_1, term_2, follow_first_for_double_variable, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            free(result);
            return unique_malloc_ptr<term_t>(nullptr);
        }
        return unique_malloc_ptr<term_t>(result);
    }

    unique_malloc_ptr<rule_t> text_to_rule(const char* text, length_t length) {
        auto result = reinterpret_cast<rule_t*>(malloc(length));
        auto scan_result = result->scan(text, reinterpret_cast<std::byte*>(result) + length);
        if (scan_result == nullptr) {
            free(result);
            return unique_malloc_ptr<rule_t>(nullptr);
        }
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<char> rule_to_text(rule_t* rule, length_t length) {
        auto result = reinterpret_cast<char*>(malloc(length));
        auto print_result = rule->print(result, reinterpret_cast<char*>(result) + length);
        if (print_result == nullptr || print_result - result == length - 1) {
            free(result);
            return unique_malloc_ptr<char>(nullptr);
        }
        *print_result = 0;
        return unique_malloc_ptr<char>(result);
    }

    unique_malloc_ptr<rule_t> ground(rule_t* rule, term_t* dictionary, length_t length) {
        auto result = reinterpret_cast<rule_t*>(malloc(length));
        if (result->ground(rule, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            free(result);
            return unique_malloc_ptr<rule_t>(nullptr);
        }
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<rule_t> ground(rule_t* rule, rule_t* dictionary, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        if (result->ground(rule, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            free(result);
            return unique_malloc_ptr<rule_t>(nullptr);
        }
        return unique_malloc_ptr<rule_t>(result);
    }

    unique_malloc_ptr<rule_t> match(rule_t* rule_1, rule_t* rule_2, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(malloc(length));
        if (result->match(rule_1, rule_2, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            free(result);
            return unique_malloc_ptr<rule_t>(nullptr);
        }
        return unique_malloc_ptr<rule_t>(result);
    }
} // namespace ds
