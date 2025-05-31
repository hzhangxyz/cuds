#include "term.h++"
#include "item.h++"
#include "list.h++"
#include "variable.h++"

#include <cstddef>

namespace cuds {
    term_type_t* term_t::type_pointer() {
        return reinterpret_cast<term_type_t*>(this);
    }

    term_type_t term_t::get_type() {
        return *type_pointer();
    }

    term_t* term_t::set_type(term_type_t type) {
        *type_pointer() = type;
        return this;
    }

    term_t* term_t::set_null() {
        set_type(term_type_t::null);
        return this;
    }

    term_t* term_t::set_variable() {
        set_type(term_type_t::variable);
        return this;
    }

    term_t* term_t::set_item() {
        set_type(term_type_t::item);
        return this;
    }

    term_t* term_t::set_list() {
        set_type(term_type_t::list);
        return this;
    }

    bool term_t::is_null() {
        return get_type() == term_type_t::null;
    }

    variable_t* term_t::variable() {
        if (get_type() == term_type_t::variable) {
            return reinterpret_cast<variable_t*>(reinterpret_cast<std::byte*>(this) + sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    item_t* term_t::item() {
        if (get_type() == term_type_t::item) {
            return reinterpret_cast<item_t*>(reinterpret_cast<std::byte*>(this) + sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    list_t* term_t::list() {
        if (get_type() == term_type_t::list) {
            return reinterpret_cast<list_t*>(reinterpret_cast<std::byte*>(this) + sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    length_t term_t::data_size() {
        if (get_type() == term_type_t::variable) {
            return sizeof(term_type_t) + variable()->data_size();
        } else if (get_type() == term_type_t::item) {
            return sizeof(term_type_t) + item()->data_size();
        } else if (get_type() == term_type_t::list) {
            return sizeof(term_type_t) + list()->data_size();
        }
        return -1;
    }

    std::byte* term_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* term_t::tail() {
        return head() + data_size();
    }

    char* term_t::print(char* buffer) {
        if (get_type() == term_type_t::variable) {
            return variable()->print(buffer);
        } else if (get_type() == term_type_t::item) {
            return item()->print(buffer);
        } else if (get_type() == term_type_t::list) {
            return list()->print(buffer);
        }
        return buffer;
    }

    const char* term_t::scan(const char* buffer) {
        if (*buffer == '\'') {
            return set_variable()->variable()->scan(buffer);
        } else if (*buffer == '(') {
            return set_list()->list()->scan(buffer);
        } else {
            return set_item()->item()->scan(buffer);
        }
    }
} // namespace cuds
