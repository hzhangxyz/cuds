#include <cstddef>

#include <ds/item.hh>
#include <ds/list.hh>
#include <ds/term.hh>
#include <ds/variable.hh>

namespace ds {
    term_type_t* term_t::type_pointer() {
        return reinterpret_cast<term_type_t*>(this);
    }

    term_type_t term_t::get_type() {
        return *type_pointer();
    }

    term_t* term_t::set_type(term_type_t type, std::byte* check_tail) {
        // 检查对象能否存下term type数据
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(this) + sizeof(term_type_t)) {
                return nullptr;
            }
        }
        *type_pointer() = type;
        return this;
    }

    term_t* term_t::set_null(std::byte* check_tail) {
        return set_type(term_type_t::null, check_tail);
    }

    term_t* term_t::set_variable(std::byte* check_tail) {
        return set_type(term_type_t::variable, check_tail);
    }

    term_t* term_t::set_item(std::byte* check_tail) {
        return set_type(term_type_t::item, check_tail);
    }

    term_t* term_t::set_list(std::byte* check_tail) {
        return set_type(term_type_t::list, check_tail);
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
        return sizeof(term_type_t);
    }

    std::byte* term_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* term_t::tail() {
        return head() + data_size();
    }

    char* term_t::print(char* buffer, char* check_tail) {
        if (get_type() == term_type_t::variable) {
            return variable()->print(buffer, check_tail);
        } else if (get_type() == term_type_t::item) {
            return item()->print(buffer, check_tail);
        } else if (get_type() == term_type_t::list) {
            return list()->print(buffer, check_tail);
        }
        return buffer;
    }

    const char* term_t::scan(const char* buffer, std::byte* check_tail) {
        // 检查对象能否存下term type数据
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(this) + sizeof(term_type_t)) {
                return nullptr;
            }
        }
        // 由于term type空间已经确认，设置term type的尾指针检查可以跳过
        if (*buffer == '\'') {
            return set_variable(nullptr)->variable()->scan(buffer, check_tail);
        } else if (*buffer == '(') {
            return set_list(nullptr)->list()->scan(buffer, check_tail);
        } else {
            return set_item(nullptr)->item()->scan(buffer, check_tail);
        }
    }
} // namespace ds
