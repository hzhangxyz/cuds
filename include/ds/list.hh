#ifndef DS_LIST_HH
#define DS_LIST_HH

#include "config.hh"

#include <cstddef>

namespace ds {
    class term_t;

    /// @brief list_t对象
    ///
    /// 内存分布:
    ///
    /// list_size : length_t
    ///
    /// term_size : length_t[list_size]
    ///
    /// term : term_t[list_size]
    ///
    /// 这里的term_size是前若干个term的累计大小
    class list_t {
        /// @brief 获取list大小的指针
        /// @return list大小的指针
        ///
        /// 可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 在设置了list size之后调用此函数可以获得正确的大小
        length_t* list_size_pointer();

        /// @brief 获取前若干个term总大小的指针
        /// @param index 若干个term的指标
        /// @return 前若干个term总大小的指针
        ///
        /// 当index = 0时, 返回的是第一个term的大小;
        /// 当index = list_size - 1时, 返回的是所有term的总大小
        ///
        /// 可以在设置了list size之后调用此函数，此时可以获得正确的指针
        ///
        /// 如果设置了term size，此时调用此函数获得的指针可以获得正确的大小
        ///
        /// 如果index = list_size，此时可以获取溢出的指针，将得到第0个term的指针
        length_t* term_size_pointer(length_t index);

        /// @brief 获取某个term的指针
        /// @param index 某个term的指标
        /// @return 某个term的指针
        ///
        /// 必须在构造了前index - 1个term的情况下调用此函数
        ///
        /// 需要注意，每次设置第index个term后，需要调用update_term_size(index)来更新term size
        term_t* term_pointer(length_t index);

      public:
        /// @brief 获取list大小
        /// @return list大小
        ///
        /// 必须在已经设置了list size的情况下调用此函数
        length_t get_list_size();

        /// @brief 设置list大小
        /// @param list_size 新的list大小
        /// @param check_tail 可选的尾指针检查
        /// @return list_t对象的指针，如果尾指针失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        list_t* set_list_size(length_t list_size, std::byte* check_tail = nullptr);

        /// @brief 获取前若干个term的总大小
        /// @param index 若干个term的指标
        /// @return 前若干个term的总大小
        ///
        /// 当index = 0时, 返回的是第一个term的大小;
        /// 当index = list_size - 1时, 返回的是所有term的总大小
        ///
        /// 必须在设置了对应term size之后调用此函数。
        length_t term_size(length_t index);

        /// @brief 获取某个term的指针
        /// @param index 某个term的指标
        /// @return 某个term的指针
        ///
        /// 必须在设置好了前index - 1个term的情况下调用此函数
        term_t* term(length_t index);

        /// @brief 更新term_size当中的某个元素
        /// @param index 某个元素的指标
        ///
        /// 此函数会根据上一个term_size与此term的size更新下一个累计大小,
        /// 通常的工作流是:
        /// 设置第0个term, 更新第0个term size;
        /// 设置第1个term, 更新第1个term size;
        /// 设置第2个term, 更新第2个term size;
        /// ...
        void update_term_size(length_t index);

        /// @brief 获取list_t对象的大小
        /// @return list_t对象的大小
        length_t data_size();

        /// @brief 获取list_t对象的头字节指针
        /// @return list_t对象的头字节指针
        std::byte* head();

        /// @brief 获取list_t对象的尾字节指针
        /// @return list_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将list_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输出后的buffer指针，如果尾指针检查失败则返回nullptr
        char* print(char* buffer, char* check_tail = nullptr);

        /// @brief 从buffer中输入list_t对象
        /// @param buffer 待输入的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输入后的buffer指针，如果尾指针检查失败则返回nullptr
        const char* scan(const char* buffer, std::byte* check_tail = nullptr);
    };
} // namespace ds

#endif
