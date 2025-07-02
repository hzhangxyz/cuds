#ifndef DS_STRING_HXX
#define DS_STRING_HXX

#include "config.hh"

#include <cstddef>

namespace ds {
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
        ///
        /// 可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 在设置了长度之后调用此函数可以获得正确的长度
        length_t* length_pointer();

        /// @brief 获取字符串本身的指针
        /// @return 字符串本身的指针
        ///
        /// 可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 在设置了字符串数据之后调用此函数可以获得正确的字符串内容
        char* string_pointer();

      public:
        /// @brief 获取字符串的长度
        /// @return 字符串的长度
        ///
        /// 可以在设置长度，但没有设置字符串数据的情况下调用此函数
        length_t get_length();

        /// @brief 设置字符串的长度
        /// @param length 字符串的新长度
        /// @param check_tail 可选的尾指针检查
        /// @return string_t对象的指针，如果尾指针失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        string_t* set_length(length_t length, std::byte* check_tail = nullptr);

        /// @brief 获取字符串本身
        /// @return 字符串本身
        ///
        /// 可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 在设置了字符串数据之后调用此函数可以获得正确的字符串内容
        char* get_string();

        /// @brief 设置字符串本身
        /// @param buffer 新的字符串
        /// @return string_t对象的指针
        ///
        /// 超出长度的部分将被截断
        ///
        /// 一定需要先已经设置了长度才可以调用此函数，因此此函数不需要做尾指针检查
        string_t* set_string(const char* buffer);

        /// @brief 设置字符串本身
        /// @param buffer null结尾的新的字符串
        /// @param check_tail 可选的尾指针检查
        /// @return string_t对象的指针，如果尾指针失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        string_t* set_null_string(const char* buffer, std::byte* check_tail = nullptr);

        /// @brief 获取string_t对象的大小
        /// @return string_t对象的大小
        length_t data_size();

        /// @brief 获取string_t对象的头字节指针
        /// @return string_t对象的头字节指针
        std::byte* head();

        /// @brief 获取string_t对象的尾字节指针
        /// @return string_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将string_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输出后的buffer指针，如果尾指针检查失败则返回nullptr
        char* print(char* buffer, char* check_tail = nullptr);

        /// @brief 从buffer中输入string_t对象
        /// @param buffer 待输入的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输入后的buffer指针，如果尾指针检查失败则返回nullptr
        const char* scan(const char* buffer, std::byte* check_tail = nullptr);
    };
} // namespace ds

#endif
