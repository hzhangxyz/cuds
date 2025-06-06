#include "helper.h++"
#include "item.h++"
#include "list.h++"
#include "term.h++"
#include "variable.h++"

#include <cstddef>

namespace cuds {
    CUDA_HOST_DEVICE term_type_t* term_t::type_pointer() {
        return reinterpret_cast<term_type_t*>(this);
    }

    CUDA_HOST_DEVICE term_type_t term_t::get_type() {
        return *type_pointer();
    }

    CUDA_HOST_DEVICE term_t* term_t::set_type(term_type_t type) {
        *type_pointer() = type;
        return this;
    }

    CUDA_HOST_DEVICE term_t* term_t::set_null() {
        set_type(term_type_t::null);
        return this;
    }

    CUDA_HOST_DEVICE term_t* term_t::set_variable() {
        set_type(term_type_t::variable);
        return this;
    }

    CUDA_HOST_DEVICE term_t* term_t::set_item() {
        set_type(term_type_t::item);
        return this;
    }

    CUDA_HOST_DEVICE term_t* term_t::set_list() {
        set_type(term_type_t::list);
        return this;
    }

    CUDA_HOST_DEVICE bool term_t::is_null() {
        return get_type() == term_type_t::null;
    }

    CUDA_HOST_DEVICE variable_t* term_t::variable() {
        if (get_type() == term_type_t::variable) {
            return with_offset<variable_t>(this, sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    CUDA_HOST_DEVICE item_t* term_t::item() {
        if (get_type() == term_type_t::item) {
            return with_offset<item_t>(this, sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    CUDA_HOST_DEVICE list_t* term_t::list() {
        if (get_type() == term_type_t::list) {
            return with_offset<list_t>(this, sizeof(term_type_t));
        } else {
            return nullptr;
        }
    }

    CUDA_HOST_DEVICE length_t term_t::data_size() {
        if (get_type() == term_type_t::variable) {
            return sizeof(term_type_t) + variable()->data_size();
        } else if (get_type() == term_type_t::item) {
            return sizeof(term_type_t) + item()->data_size();
        } else if (get_type() == term_type_t::list) {
            return sizeof(term_type_t) + list()->data_size();
        }
        return -1;
    }

    CUDA_HOST_DEVICE std::byte* term_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    CUDA_HOST_DEVICE std::byte* term_t::tail() {
        return head() + data_size();
    }

    CUDA_HOST_DEVICE char* term_t::print(char* buffer) {
        if (get_type() == term_type_t::variable) {
            return variable()->print(buffer);
        } else if (get_type() == term_type_t::item) {
            return item()->print(buffer);
        } else if (get_type() == term_type_t::list) {
            return list()->print(buffer);
        }
        return buffer;
    }

    CUDA_HOST_DEVICE const char* term_t::scan(const char* buffer) {
        if (*buffer == '\'') {
            return set_variable()->variable()->scan(buffer);
        } else if (*buffer == '(') {
            return set_list()->list()->scan(buffer);
        } else {
            return set_item()->item()->scan(buffer);
        }
    }
} // namespace cuds
