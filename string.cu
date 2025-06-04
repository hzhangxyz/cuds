#include "cuda_compact.h++"
#include "string.h++"

namespace cuds {
    CUDA_HOST_DEVICE length_t* string_t::length_pointer() {
        return reinterpret_cast<length_t*>(this);
    }

    CUDA_HOST_DEVICE char* string_t::string_pointer() {
        return reinterpret_cast<char*>(reinterpret_cast<std::byte*>(this) + sizeof(length_t));
    }

    CUDA_HOST_DEVICE length_t string_t::get_length() {
        return *length_pointer();
    }

    CUDA_HOST_DEVICE string_t* string_t::set_length(length_t length) {
        *length_pointer() = length;
        return this;
    }

    CUDA_HOST_DEVICE char* string_t::get_string() {
        return string_pointer();
    }

    CUDA_HOST_DEVICE string_t* string_t::set_string(const char* buffer) {
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

    CUDA_HOST_DEVICE string_t* string_t::set_null_string(const char* buffer) {
        set_length(strlen(buffer) + 1);
        set_string(buffer);
        return this;
    }

    CUDA_HOST_DEVICE length_t string_t::data_size() {
        return sizeof(length_t) + sizeof(char) * get_length();
    }

    CUDA_HOST_DEVICE std::byte* string_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    CUDA_HOST_DEVICE std::byte* string_t::tail() {
        return head() + data_size();
    }

    CUDA_HOST_DEVICE char* string_t::print(char* buffer) {
        char* src = get_string();
        while (*src) {
            *(buffer++) = *(src++);
        }
        return buffer;
    }

    CUDA_HOST_DEVICE const char* string_t::scan(const char* buffer) {
        const char* src = buffer;
        char* dst = get_string();
        while (true) {
            if (strchr("'() \t\r\n", *src) != nullptr) {
                break;
            }
            *(dst++) = *(src++);
        }
        *dst = 0;
        set_length(strlen(get_string()) + 1);
        return src;
    }
} // namespace cuds
