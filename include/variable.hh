#ifndef DS_VARIABLE_HXX
#define DS_VARIABLE_HXX

#include "config.hh"
#include "string.hh"

#include <cstddef>

namespace ds {
    /// @brief variable_t对象
    ///
    /// 内存分布:
    ///
    /// name : string_t
    class variable_t {
      public:
        /// @brief 获取variable的name
        /// @return variable的name
        string_t* name();

        /// @brief 获取variable_t对象的大小
        /// @return variable_t对象的大小
        length_t data_size();

        /// @brief 获取variable_t对象的头字节指针
        /// @return variable_t对象的头字节指针
        std::byte* head();

        /// @brief 获取variable_t对象的尾字节指针
        /// @return variable_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将variable_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @return 被输出后的buffer指针
        char* print(char* buffer);

        /// @brief 从buffer中输入variable_t对象
        /// @param buffer 待输入的buffer指针
        /// @return 被输入后的buffer指针
        const char* scan(const char* buffer);
    };
} // namespace ds

#endif
