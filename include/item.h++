#ifndef CUDS_ITEM_HXX
#define CUDS_ITEM_HXX

#include "config.h++"
#include "string.h++"

#include <cstddef>

namespace cuds {
    /// @brief item_t对象
    ///
    /// 内存分布:
    ///
    /// name : string_t
    class item_t {
      public:
        /// @brief 获取item的name
        /// @return item的name
        CUDA_HOST_DEVICE string_t* name();

        /// @brief 获取item_t对象的大小
        /// @return item_t对象的大小
        CUDA_HOST_DEVICE length_t data_size();

        /// @brief 获取item_t对象的头字节指针
        /// @return item_t对象的头字节指针
        CUDA_HOST_DEVICE std::byte* head();

        /// @brief 获取item_t对象的尾字节指针
        /// @return item_t对象的尾字节指针
        CUDA_HOST_DEVICE std::byte* tail();

        /// @brief 将item_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @return 被输出后的buffer指针
        CUDA_HOST_DEVICE char* print(char* buffer);

        /// @brief 从buffer中输入item_t对象
        /// @param buffer 待输入的buffer指针
        /// @return 被输入后的buffer指针
        CUDA_HOST_DEVICE const char* scan(const char* buffer);
    };
} // namespace cuds

#endif
