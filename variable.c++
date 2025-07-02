#include "variable.h++"

namespace ds {
    string_t* variable_t::name() {
        return reinterpret_cast<string_t*>(this);
    }

    length_t variable_t::data_size() {
        return name()->data_size();
    }

    std::byte* variable_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* variable_t::tail() {
        return head() + data_size();
    }

    char* variable_t::print(char* buffer) {
        *(buffer++) = '\'';
        return name()->print(buffer);
    }

    const char* variable_t::scan(const char* buffer) {
        return name()->scan(buffer + 1);
    }
} // namespace ds
