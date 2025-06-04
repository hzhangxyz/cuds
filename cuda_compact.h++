#ifndef CUDS_CUDA_COMPAT_HXX
#define CUDS_CUDA_COMPAT_HXX

#include "config.h++"

#include <cstdint>

namespace cuds {
    CUDA_HOST_DEVICE char* strchr(char* str, int ch);

    CUDA_HOST_DEVICE const char* strchr(const char* str, int ch);

    CUDA_HOST_DEVICE std::size_t strlen(const char* str);

    CUDA_HOST_DEVICE int strcmp(const char* str1, const char* str2);
} // namespace cuds

#endif
