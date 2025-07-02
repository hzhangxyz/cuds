#include "rule.h++"

#include <cstring>

namespace ds {
    term_t* rule_t::conclusion() {
        return term(get_list_size() - 1);
    }

    term_t* rule_t::only_conclusion() {
        if (premises_count() == 0) {
            return conclusion();
        } else {
            return nullptr;
        }
    }

    term_t* rule_t::premises(length_t index) {
        return term(index);
    }

    length_t rule_t::premises_count() {
        return get_list_size() - 1;
    }

    bool rule_t::valid() {
        return get_list_size() != 0;
    }

    length_t rule_t::data_size() {
        return list_t::data_size();
    }

    std::byte* rule_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* rule_t::tail() {
        return head() + data_size();
    }

    char* rule_t::print(char* buffer) {
        length_t max_length = 4;
        char* last_buffer = buffer;
        for (length_t index = 0; index < premises_count(); ++index) {
            buffer = premises(index)->print(buffer);
            length_t this_length = buffer - last_buffer;
            max_length = this_length > max_length ? this_length : max_length;
            *(buffer++) = '\n';
            last_buffer = buffer;
        }
        for (length_t length = 0; length < max_length; ++length) {
            *(buffer++) = '-';
        }
        *(buffer++) = '\n';
        buffer = conclusion()->print(buffer);
        *(buffer++) = '\n';
        return buffer;
    }

    const char* rule_t::scan(const char* buffer) {
        term_t* term = reinterpret_cast<term_t*>(this);
        length_t list_size = 0;
        bool last_one = false;
        while (buffer) {
            if (*buffer == 0) {
                break;
            }
            if (strchr(" \t\r\n", *buffer)) {
                ++buffer;
                continue;
            }
            if (buffer[0] == '-' && buffer[1] == '-') {
                while (*(buffer++) == '-') {
                }
                last_one = true;
                continue;
            }
            buffer = term->scan(buffer);
            term = reinterpret_cast<term_t*>(term->tail());
            ++list_size;
            if (last_one) {
                break;
            }
        }
        length_t offset = sizeof(length_t) + sizeof(length_t) * list_size;
        memcpy(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(this),
            reinterpret_cast<std::byte*>(term) - reinterpret_cast<std::byte*>(this)
        );
        set_list_size(list_size);
        for (length_t index = 0; index < list_size; ++index) {
            update_term_size(index);
        }
        return buffer;
    }
} // namespace ds
