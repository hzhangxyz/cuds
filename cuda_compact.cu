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

    CUDA_HOST_DEVICE int memcmp(const void* s1, const void* s2, std::size_t n) {
#ifdef __CUDA_ARCH__
        const unsigned char* p1 = reinterpret_cast<const unsigned char*>(s1);
        const unsigned char* p2 = reinterpret_cast<const unsigned char*>(s2);
        for (std::size_t i = 0; i < n; i++) {
            if (p1[i] != p2[i]) {
                return (p1[i] > p2[i]) ? 1 : -1;
            }
        }
        return 0;
#else
        return std::memcmp(s1, s2, n);
#endif
    }

    CUDA_HOST_DEVICE void* memmove(void* dst, const void* src, std::size_t n) {
#ifdef __CUDA_ARCH__
        unsigned char* d = (unsigned char*)dst;
        const unsigned char* s = (const unsigned char*)src;
        if (d == s) {
            return dst;
        }
        if (s < d && d < s + n) {
            for (size_t i = n; i > 0; i--) {
                d[i - 1] = s[i - 1];
            }
        } else {
            for (size_t i = 0; i < n; i++) {
                d[i] = s[i];
            }
        }
        return dst;
#else
        return std::memmove(dst, src, n);
#endif
    }

    CUDA_HOST_DEVICE length_t get_ensure_align(length_t* ptr) {
#ifdef __CUDA_ARCH__
        length_t result;
        memcpy(&result, ptr, sizeof(length_t));
        return result;
#else
        return *ptr;
#endif
    }

    CUDA_HOST_DEVICE length_t* set_ensure_align(length_t* ptr, length_t value) {
#ifdef __CUDA_ARCH__
        memcpy(ptr, &value, sizeof(length_t));
#else
        *ptr = value;
#endif
        return ptr;
    }
} // namespace cuds
