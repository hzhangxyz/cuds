#include "item.h++"

namespace ds {
    string_t* item_t::name() {
        return reinterpret_cast<string_t*>(this);
    }

    length_t item_t::data_size() {
        return name()->data_size();
    }

    std::byte* item_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* item_t::tail() {
        return head() + data_size();
    }

    char* item_t::print(char* buffer) {
        return name()->print(buffer);
    }

    const char* item_t::scan(const char* buffer) {
        return name()->scan(buffer);
    }
} // namespace ds
