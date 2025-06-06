#ifndef CUDS_HELPER_HXX
#define CUDS_HELPER_HXX

#include "config.h++"

#include <type_traits>
#include <cstddef>

namespace cuds {
    template<typename R = void, typename T>
    CUDA_HOST_DEVICE auto with_offset(T* ptr, std::size_t offset = 0) {
        return reinterpret_cast<std::conditional_t<std::is_same_v<R, void>, T, R>*>(reinterpret_cast<std::byte*>(ptr) + offset);
    }
} // namespace cuds

#endif
