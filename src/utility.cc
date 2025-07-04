#include <cstdlib>

#include <ds/list.hh>
#include <ds/utility.hh>

namespace ds {
    std::unique_ptr<term_t> text_to_term(const char* text, length_t length) {
        auto result = reinterpret_cast<term_t*>(operator new(length));
        auto scan_result = result->scan(text, reinterpret_cast<std::byte*>(result) + length);
        if (scan_result == nullptr) {
            operator delete(result);
            return std::unique_ptr<term_t>(nullptr);
        }
        return std::unique_ptr<term_t>(result);
    }

    std::unique_ptr<char> term_to_text(term_t* term, length_t length) {
        auto result = reinterpret_cast<char*>(operator new(length));
        auto print_result = term->print(result, reinterpret_cast<char*>(result) + length);
        if (print_result == nullptr || print_result - result == length - 1) {
            operator delete(result);
            return std::unique_ptr<char>(nullptr);
        }
        *print_result = 0;
        return std::unique_ptr<char>(result);
    }

    std::unique_ptr<term_t> ground(term_t* term, term_t* dictionary, length_t length) {
        auto result = reinterpret_cast<term_t*>(operator new(length));
        if (result->ground(term, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            operator delete(result);
            return std::unique_ptr<term_t>(nullptr);
        }
        return std::unique_ptr<term_t>(result);
    }

    std::unique_ptr<term_t> match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, length_t length) {
        auto result = reinterpret_cast<term_t*>(operator new(length));
        if (result->match(term_1, term_2, follow_first_for_double_variable, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            operator delete(result);
            return std::unique_ptr<term_t>(nullptr);
        }
        return std::unique_ptr<term_t>(result);
    }

    std::unique_ptr<rule_t> text_to_rule(const char* text, length_t length) {
        auto result = reinterpret_cast<rule_t*>(operator new(length));
        auto scan_result = result->scan(text, reinterpret_cast<std::byte*>(result) + length);
        if (scan_result == nullptr) {
            operator delete(result);
            return std::unique_ptr<rule_t>(nullptr);
        }
        return std::unique_ptr<rule_t>(result);
    }

    std::unique_ptr<char> rule_to_text(rule_t* rule, length_t length) {
        auto result = reinterpret_cast<char*>(operator new(length));
        auto print_result = rule->print(result, reinterpret_cast<char*>(result) + length);
        if (print_result == nullptr || print_result - result == length - 1) {
            operator delete(result);
            return std::unique_ptr<char>(nullptr);
        }
        *print_result = 0;
        return std::unique_ptr<char>(result);
    }

    std::unique_ptr<rule_t> ground(rule_t* rule, term_t* dictionary, length_t length) {
        auto result = reinterpret_cast<rule_t*>(operator new(length));
        if (result->ground(rule, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            operator delete(result);
            return std::unique_ptr<rule_t>(nullptr);
        }
        return std::unique_ptr<rule_t>(result);
    }

    std::unique_ptr<rule_t> ground(rule_t* rule, rule_t* dictionary, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(operator new(length));
        if (result->ground(rule, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            operator delete(result);
            return std::unique_ptr<rule_t>(nullptr);
        }
        return std::unique_ptr<rule_t>(result);
    }

    std::unique_ptr<rule_t> match(rule_t* rule_1, rule_t* rule_2, length_t length) {
        rule_t* result = reinterpret_cast<rule_t*>(operator new(length));
        if (result->match(rule_1, rule_2, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
            operator delete(result);
            return std::unique_ptr<rule_t>(nullptr);
        }
        return std::unique_ptr<rule_t>(result);
    }
} // namespace ds
