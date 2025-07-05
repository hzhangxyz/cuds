#ifndef DS_ITEM_HH
#define DS_ITEM_HH

#include "config.hh"
#include "string.hh"

#include <cstddef>

namespace ds {
    /// @brief item_t对象
    ///
    /// 内存分布:
    ///
    /// name : string_t
    class item_t {
      public:
        /// @brief 获取item的name
        /// @return item的name
        ///
        /// 可以在构造前的对象中调用此函数
        string_t* name();

        /// @brief 获取item_t对象的大小
        /// @return item_t对象的大小
        length_t data_size();

        /// @brief 获取item_t对象的头字节指针
        /// @return item_t对象的头字节指针
        std::byte* head();

        /// @brief 获取item_t对象的尾字节指针
        /// @return item_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将item_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输出后的buffer指针，如果尾指针检查失败则返回nullptr
        char* print(char* buffer, char* check_tail = nullptr);

        /// @brief 从buffer中输入item_t对象
        /// @param buffer 待输入的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输入后的buffer指针，如果尾指针检查失败则返回nullptr
        const char* scan(const char* buffer, std::byte* check_tail = nullptr);
    };
} // namespace ds

#endif
