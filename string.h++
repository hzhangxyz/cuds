#ifndef CUDS_STRING_HXX
#define CUDS_STRING_HXX

#include "config.h++"

#include <cstddef>

namespace cuds {
    /// @brief string_t对象
    ///
    /// 内存分布:
    ///
    /// length : length_t
    ///
    /// string : char[length]
    class string_t {
        /// @brief 获取字符串长度的指针
        /// @return 字符串长度的指针
        CUDA_HOST_DEVICE length_t* length_pointer();

        /// @brief 获取字符串本身的指针
        /// @return 字符串本身的指针
        CUDA_HOST_DEVICE char* string_pointer();

      public:
        /// @brief 获取字符串的长度
        /// @return 字符串的长度
        CUDA_HOST_DEVICE length_t get_length();

        /// @brief 设置字符串的长度
        /// @param length 字符串的新长度
        /// @return string_t对象的指针
        CUDA_HOST_DEVICE string_t* set_length(length_t length);

        /// @brief 获取字符串本身
        /// @return 字符串本身
        CUDA_HOST_DEVICE char* get_string();

        /// @brief 设置字符串本身
        /// @param buffer 新的字符串
        /// @return string_t对象的指针
        ///
        /// 超出长度的部分将被截断
        CUDA_HOST_DEVICE string_t* set_string(const char* buffer);

        /// @brief 设置字符串本身
        /// @param buffer null结尾的新的字符串
        /// @return string_t对象的指针
        CUDA_HOST_DEVICE string_t* set_null_string(const char* buffer);

        /// @brief 获取string_t对象的大小
        /// @return string_t对象的大小
        CUDA_HOST_DEVICE length_t data_size();

        /// @brief 获取string_t对象的头字节指针
        /// @return string_t对象的头字节指针
        CUDA_HOST_DEVICE std::byte* head();

        /// @brief 获取string_t对象的尾字节指针
        /// @return string_t对象的尾字节指针
        CUDA_HOST_DEVICE std::byte* tail();

        /// @brief 将string_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @return 被输出后的buffer指针
        CUDA_HOST_DEVICE char* print(char* buffer);

        /// @brief 从buffer中输入string_t对象
        /// @param buffer 待输入的buffer指针
        /// @return 被输入后的buffer指针
        CUDA_HOST_DEVICE const char* scan(const char* buffer);
    };
} // namespace cuds

#endif
