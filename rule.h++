#ifndef DS_RULE_HXX
#define DS_RULE_HXX

#include "list.h++"
#include "term.h++"

namespace ds {
    class rule_t : private list_t {
      public:
        /// @brief 获取rule的conclusion
        /// @return rule的conclusion
        term_t* conclusion();

        /// @brief 获取rule仅有的conclusion
        /// @return rule仅有的conclusion, 如果rule含有premises, 则返回nullptr
        term_t* only_conclusion();

        /// @brief 获取rule的某个premises
        /// @param index 某个preimises的指标
        /// @return rule的某个premises
        term_t* premises(length_t index);

        /// @brief 获取rule的premises数目
        /// @return rule的premises数目
        length_t premises_count();

        /// @brief 判断rule的合法性
        /// @return rule的合法性
        bool valid();

        /// @brief 获取rule_t对象的大小
        /// @return rule_t对象的大小
        length_t data_size();

        /// @brief 获取rule_t对象的头字节指针
        /// @return rule_t对象的头字节指针
        std::byte* head();

        /// @brief 获取rule_t对象的尾字节指针
        /// @return rule_t对象的尾字节指针
        std::byte* tail();

        /// @brief 将rule_t对象输出至buffer
        /// @param buffer 待被输出的buffer指针
        /// @return 被输出后的buffer指针
        char* print(char* buffer);

        /// @brief 从buffer中输入rule_t对象
        /// @param buffer 待输入的buffer指针
        /// @return 被输入后的buffer指针
        const char* scan(const char* buffer);

        /// @brief 将rule使用dictionary进行ground, 结果更新至本对象
        /// @param rule 待被ground的rule
        /// @param dictionary 含有list of pair的term作为的dictionary, pair的两个部分分别是key和value
        /// @return rule_t对象的指针
        rule_t* ground(rule_t* rule, term_t* dictionary);

        /// @brief 将rule使用dictionary进行ground, 结果更新至本对象
        /// @param rule 待被ground的rule
        /// @param dictionary 含有list of pair的term的rule作为的dictionary, pair的两个部分分别是key和value
        /// @return rule_t对象的指针
        rule_t* ground(rule_t* rule, rule_t* dictionary);

        /// @brief 将两个rule尽可能相互提示并ground, 随后进行apply, 结果更新至本对象
        /// @param rule_1 待被apply的rule
        /// @param rule_2 待作为fact的rule
        /// @return rule_t对象的指针
        rule_t* match(rule_t* rule_1, rule_t* rule_2);
    };
} // namespace ds

#endif
