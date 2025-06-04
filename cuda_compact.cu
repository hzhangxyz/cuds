#include "cuda_compact.h++"

#include <cstring>

namespace cuds {
    CUDA_HOST_DEVICE char* strchr(char* str, int ch) {
#ifdef __CUDA_ARCH__
        while (*str != 0) {
            if (*str == (char)ch) {
                return str;
            }
            str++;
        }

        if (ch == 0) {
            return str;
        }

        return nullptr;
#else
        return std::strchr(str, ch);
#endif
    }

    CUDA_HOST_DEVICE const char* strchr(const char* str, int ch) {
#ifdef __CUDA_ARCH__
        while (*str != 0) {
            if (*str == (char)ch) {
                return str;
            }
            str++;
        }

        if (ch == 0) {
            return str;
        }

        return nullptr;
#else
        return std::strchr(str, ch);
#endif
    }

    CUDA_HOST_DEVICE std::size_t strlen(const char* str) {
#ifdef __CUDA_ARCH__
        const char* p = str;
        while (*p != 0) {
            p++;
        }
        return p - str;
#else
        return std::strlen(str);
#endif
    }

    CUDA_HOST_DEVICE int strcmp(const char* str1, const char* str2) {
#ifdef __CUDA_ARCH__
        while (*str1 != 0 && *str1 == *str2) {
            str1++;
            str2++;
        }
        return static_cast<unsigned char>(*str1) - static_cast<unsigned char>(*str2);
#else
        return std::strcmp(str1, str2);
#endif
    }
} // namespace cuds
