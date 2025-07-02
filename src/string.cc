#include <cstring>

#include <ds/string.hh>

namespace ds {
    length_t* string_t::length_pointer() {
        return reinterpret_cast<length_t*>(this);
    }

    char* string_t::string_pointer() {
        return reinterpret_cast<char*>(reinterpret_cast<std::byte*>(this) + sizeof(length_t));
    }

    length_t string_t::get_length() {
        return *length_pointer();
    }

    string_t* string_t::set_length(length_t length, std::byte* check_tail) {
        // 检查对象能否存下长度数据
        // 考虑极限情况，长度为0（虽然字符串长度不可能为0,有\0的尾巴）时，check_tail == get_string()
        // 所以这里我们只需要保证 check_tail >= get_string() 即可
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(get_string())) {
                return nullptr;
            }
        }
        *length_pointer() = length;
        // 检查对象能否存下字符串数据
        // tail是最后一字节数据后面的那个位置，所以check_tail == tail()是可以的
        if (check_tail != nullptr) {
            if (check_tail < tail()) {
                return nullptr;
            }
        }
        return this;
    }

    char* string_t::get_string() {
        return string_pointer();
    }

    string_t* string_t::set_string(const char* buffer) {
        const char* src = buffer;
        char* dst = get_string();
        bool end = false;
        for (length_t index = 0; index < get_length() - 1; ++index) {
            if (end) {
                dst[index] = 0;
            } else {
                dst[index] = src[index];
                if (src[index] == 0) {
                    end = true;
                }
            }
        }
        dst[get_length() - 1] = 0;
        return this;
    }

    string_t* string_t::set_null_string(const char* buffer, std::byte* check_tail) {
        set_length(strlen(buffer) + 1, check_tail);
        set_string(buffer);
        return this;
    }

    length_t string_t::data_size() {
        return sizeof(length_t) + sizeof(char) * get_length();
    }

    std::byte* string_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* string_t::tail() {
        return head() + data_size();
    }

    char* string_t::print(char* buffer, char* check_tail) {
        char* src = get_string();
        while (*src) {
            // 写前检查，如果buffer已经超过了check_tail(等于也会失败)，则失败
            if (check_tail != nullptr) {
                if (check_tail <= buffer) {
                    return nullptr;
                }
            }
            *(buffer++) = *(src++);
        }
        return buffer;
    }

    const char* string_t::scan(const char* buffer, std::byte* check_tail) {
        const char* src = buffer;
        char* dst = get_string();
        while (true) {
            if (strchr("'() \t\r\n", *src) != nullptr) {
                break;
            }
            // 写前检查，如果dst已经超过了check_tail(等于也会失败)，则失败
            if (check_tail != nullptr) {
                if (check_tail <= reinterpret_cast<std::byte*>(dst)) {
                    return nullptr;
                }
            }
            *(dst++) = *(src++);
        }
        // 最后一个字符是'\0'，也需要检查是否超过了check_tail
        if (check_tail != nullptr) {
            if (check_tail <= reinterpret_cast<std::byte*>(dst)) {
                return nullptr;
            }
        }
        *dst = 0;
        // 已经在后面放置数据了，不需要检查前面的空间是否足够
        set_length(strlen(get_string()) + 1, nullptr);
        return src;
    }
} // namespace ds
