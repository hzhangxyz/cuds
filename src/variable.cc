#include <ds/variable.hh>

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

    char* variable_t::print(char* buffer, char* check_tail) {
        // 因为在写string前会先写一个单引号，所以在这里也需要检查check_tail是否足够
        if (check_tail != nullptr) {
            if (check_tail <= buffer) {
                return nullptr;
            }
        }
        *(buffer++) = '\'';
        return name()->print(buffer, check_tail);
    }

    const char* variable_t::scan(const char* buffer, std::byte* check_tail) {
        return name()->scan(buffer + 1, check_tail);
    }
} // namespace ds
