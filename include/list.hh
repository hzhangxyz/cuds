#ifndef DS_LIST_HXX
#define DS_LIST_HXX

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
        length_t* list_size_pointer();

        /// @brief 获取前若干个term总大小的指针
        /// @param index 若干个term的指标
        /// @return 前若干个term总大小的指针
        ///
        /// 当index = 0时, 返回的是第一个term的大小;
        /// 当index = list_size - 1时, 返回的是所有term的总大小
        length_t* term_size_pointer(length_t index);

        /// @brief 获取某个term的指针
        /// @param index 某个term的指标
        /// @return 某个term的指针
        term_t* term_pointer(length_t index);

      public:
        /// @brief 获取list大小
        /// @return list大小
        length_t get_list_size();

        /// @brief 设置list大小
        /// @param list_size 新的list大小
        /// @return list_t对象的指针
        list_t* set_list_size(length_t list_size);

        /// @brief 获取前若干个term的总大小
        /// @param index 若干个term的指标
        /// @return 前若干个term的总大小
        ///
        /// 当index = 0时, 返回的是第一个term的大小;
        /// 当index = list_size - 1时, 返回的是所有term的总大小
        length_t term_size(length_t index);

        /// @brief 获取某个term的指针
        /// @param index 某个term的指标
        /// @return 某个term的指针
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
        /// @return 被输出后的buffer指针
        char* print(char* buffer);

        /// @brief 从buffer中输入list_t对象
        /// @param buffer 待输入的buffer指针
        /// @return 被输入后的buffer指针
        const char* scan(const char* buffer);
    };
} // namespace ds

#endif
