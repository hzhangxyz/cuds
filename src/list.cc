#include "list.hh"
#include "term.hh"

#include <cstring>

namespace ds {
    length_t* list_t::list_size_pointer() {
        return reinterpret_cast<length_t*>(this);
    }

    length_t* list_t::term_size_pointer(length_t index) {
        return reinterpret_cast<length_t*>(reinterpret_cast<std::byte*>(this) + sizeof(length_t) + sizeof(length_t) * index);
    }

    term_t* list_t::term_pointer(length_t index) {
        if (index == 0) {
            return reinterpret_cast<term_t*>(term_size_pointer(get_list_size()));
        } else if (index < 0 || index >= get_list_size()) {
            return nullptr;
        } else {
            return reinterpret_cast<term_t*>(reinterpret_cast<std::byte*>(term_size_pointer(get_list_size())) + term_size(index - 1));
        }
    }

    length_t list_t::get_list_size() {
        return *list_size_pointer();
    }

    list_t* list_t::set_list_size(length_t list_size) {
        *list_size_pointer() = list_size;
        for (length_t index = 0; index < get_list_size(); ++index) {
            *term_size_pointer(index) = 0;
        }
        return this;
    }

    length_t list_t::term_size(length_t index) {
        return *term_size_pointer(index);
    }

    term_t* list_t::term(length_t index) {
        return term_pointer(index);
    }

    void list_t::update_term_size(length_t index) {
        if (index == 0) {
            *term_size_pointer(index) = term(index)->data_size();
        } else {
            *term_size_pointer(index) = term(index)->data_size() + *term_size_pointer(index - 1);
        }
    }

    length_t list_t::data_size() {
        return sizeof(length_t) + sizeof(length_t) * get_list_size() + term_size(get_list_size() - 1);
    }

    std::byte* list_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* list_t::tail() {
        return head() + data_size();
    }

    char* list_t::print(char* buffer) {
        *(buffer++) = '(';
        for (length_t index = 0; index < get_list_size(); ++index) {
            if (index != 0) {
                *(buffer++) = ' ';
            }
            buffer = term(index)->print(buffer);
        }
        *(buffer++) = ')';
        return buffer;
    }

    const char* list_t::scan(const char* buffer) {
        ++buffer;
        term_t* term = reinterpret_cast<term_t*>(this);
        length_t list_size = 0;
        while (true) {
            if (*buffer == ')') {
                ++buffer;
                break;
            }
            if (*buffer == '\'') {
                buffer = term->scan(buffer);
                term = reinterpret_cast<term_t*>(term->tail());
                ++list_size;
                continue;
            }
            if (strchr(" \t\r\n", *buffer)) {
                ++buffer;
                continue;
            }
            buffer = term->scan(buffer);
            term = reinterpret_cast<term_t*>(term->tail());
            ++list_size;
        }
        length_t offset = sizeof(length_t) + sizeof(length_t) * list_size;
        memmove(
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
