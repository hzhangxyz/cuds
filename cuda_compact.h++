#ifndef CUDS_CUDA_COMPAT_HXX
#define CUDS_CUDA_COMPAT_HXX

#include "config.h++"

#include <cstdint>

namespace cuds {
    CUDA_HOST_DEVICE char* strchr(char* str, int ch);

    CUDA_HOST_DEVICE const char* strchr(const char* str, int ch);

    CUDA_HOST_DEVICE std::size_t strlen(const char* str);

    CUDA_HOST_DEVICE int strcmp(const char* str1, const char* str2);

    CUDA_HOST_DEVICE int memcmp(const void* s1, const void* s2, std::size_t n);

    CUDA_HOST_DEVICE void* memmove(void* dst, const void* src, std::size_t n);

    CUDA_HOST_DEVICE length_t get_ensure_align(length_t* ptr);

    CUDA_HOST_DEVICE length_t* set_ensure_align(length_t* ptr, length_t value);
} // namespace cuds

#endif
