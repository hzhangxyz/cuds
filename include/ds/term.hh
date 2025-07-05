#ifndef DS_TERM_HH
#define DS_TERM_HH

#include "config.hh"

#include <cstddef>
#include <utility>

namespace ds {
    class variable_t;
    class item_t;
    class list_t;

    /// @brief term_t的类型enum
    enum class term_type_t : min_uint_t {
        null = 0,
        variable = 1,
        item = 2,
        list = 3
    };

    /// @brief term_t对象
    ///
    /// 内存分布:
    ///
    /// type : term_type_t
    ///
    /// data : variable | item | list
    class term_t {
        /// @brief 获取term type的指针
        /// @return term type的指针
        ///
        /// 可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 设置了term type之后调用此函数可以获得正确的指针
        term_type_t* type_pointer();

      public:
        /// @brief 获取term type
        /// @return term type
        ///
        /// 必须在设置了term type的情况下调用此函数
        term_type_t get_type();

        /// @brief 设置term type
        /// @param type 新的term type
        /// @param check_tail 可选的尾指针检查
        /// @return term_t对象的指针，如果尾指针检查失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        term_t* set_type(term_type_t type, std::byte* check_tail = nullptr);

        /// @brief 设置term type为null, 表示未初始化
        /// @param check_tail 可选的尾指针检查
        /// @return term_t对象的指针，如果尾指针检查失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        term_t* set_null(std::byte* check_tail = nullptr);

        /// @brief 设置term type为variable
        /// @param check_tail 可选的尾指针检查
        /// @return term_t对象的指针，如果尾指针检查失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        term_t* set_variable(std::byte* check_tail = nullptr);

        /// @brief 设置term type为item
        /// @param check_tail 可选的尾指针检查
        /// @return term_t对象的指针，如果尾指针检查失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        term_t* set_item(std::byte* check_tail = nullptr);

        /// @brief 设置term type为list
        /// @param check_tail 可选的尾指针检查
        /// @return term_t对象的指针，如果尾指针检查失败则返回nullptr
        ///
        /// 可以在构造前的对象中调用此函数
        term_t* set_list(std::byte* check_tail = nullptr);

        /// @brief 检查term是否为null
        /// @return 如果为null, 则返回true, 否则是false
        bool is_null();

        /// @brief 获取variable_t对象
        /// @return variable_t对象的指针
        ///
        /// 如果term中并不是variable则返回nullptr
        ///
        /// 只要设置了正确的term type，就可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 设置了正确的term后，调用此函数可以获得正确的variable_t对象
        variable_t* variable();

        /// @brief 获取item_t对象
        /// @return item_t对象的指针
        ///
        /// 如果term中并不是item则返回nullptr
        ///
        /// 只要设置了正确的term type，就可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 设置了正确的term后，调用此函数可以获得正确的item_t对象
        item_t* item();

        /// @brief 获取list_t对象
        /// @return list_t对象的指针
        ///
        /// 如果term中并不是list则返回nullptr
        ///
        /// 只要设置了正确的term type，就可以在构造前的对象中调用此函数，此时只能获得正确的指针
        ///
        /// 设置了正确的term后，调用此函数可以获得正确的list_t对象
        list_t* list();

        /// @brief 获取term_t对象的大小
        /// @return term_t对象的大小
        length_t data_size();

        /// @brief 获取term_t对象的头字节指针
        /// @return term_t对象的头字节指针
        std::byte* head();

        /// @brief 获取term_t对象的尾字节指针
        /// @return term_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将term_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输出后的buffer指针，如果尾指针检查失败则返回nullptr
        char* print(char* buffer, char* check_tail = nullptr);

        /// @brief 从buffer中输入term_t对象
        /// @param buffer 待输入的buffer指针
        /// @param check_tail 可选的尾指针检查
        /// @return 被输入后的buffer指针，如果尾指针检查失败则返回nullptr
        const char* scan(const char* buffer, std::byte* check_tail = nullptr);

        /// @brief 将term使用dictionary进行ground, 结果更新至本对象
        /// @param term 待被ground的term
        /// @param dictionary 含有list of pair的term作为的dictionary, pair的两个部分分别是key和value
        /// @param check_tail 可选的尾指针检查
        /// @return 自身，是一个term_t对象的指针，如果尾指针检查失败则返回nullptr
        term_t* ground(term_t* term, term_t* dictionary, std::byte* check_tail = nullptr);

        /// @brief 将term_1匹配term_2
        /// @param term_1 第一个term
        /// @param term_2 第二个term
        /// @param follow_first_for_double_variable 当两个term都是variable时, 是否使用第一个term的名字
        /// @param check_tail 可选的尾指针检查
        /// @return 自身，是一个用于第一个term ground的dictionary，如果匹配失败则返回nullptr
        term_t* match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, std::byte* check_tail = nullptr);
    };
} // namespace ds

#endif
