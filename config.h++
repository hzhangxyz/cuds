#ifndef CUDS_CONFIG_HXX
#define CUDS_CONFIG_HXX

#include <cstdint>

#ifdef __CUDACC__
#define CUDA_HOST_DEVICE __host__ __device__
#else
#define CUDA_HOST_DEVICE
#endif

namespace cuds {
    /// @brief 用于list长度、数据长度的类型
    using length_t = std::int32_t;

    /// @brief 用于enum的类型
    using min_uint_t = std::uint8_t;
} // namespace cuds

#endif
