#include <cstring>

#include <ds/list.hh>
#include <ds/term.hh>

namespace ds {
    length_t* list_t::list_size_pointer() {
        return reinterpret_cast<length_t*>(this);
    }

    length_t* list_t::term_size_pointer(length_t index) {
        return reinterpret_cast<length_t*>(reinterpret_cast<std::byte*>(this) + sizeof(length_t) + sizeof(length_t) * index);
    }

    term_t* list_t::term_pointer(length_t index) {
        if (index == 0) {
            // term_size_pointer的输入index溢出后正好是第0个term的位置
            return reinterpret_cast<term_t*>(term_size_pointer(get_list_size()));
        } else if (index < 0 || index >= get_list_size()) {
            return nullptr;
        } else {
            return reinterpret_cast<term_t*>(reinterpret_cast<std::byte*>(term_size_pointer(get_list_size())) + term_size(index - 1));
        }
    }

    length_t list_t::get_list_size() {
        return *list_size_pointer();
    }

    list_t* list_t::set_list_size(length_t list_size, std::byte* check_tail) {
        // 检查对象能否存下list size数据
        // 考虑极限情况，list size为0时，check_tail == term_size_pointer(0)是可以的
        // 只有在check_tail比这个还小的情况下，才会出错
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(term_size_pointer(0))) {
                return nullptr;
            }
        }
        *list_size_pointer() = list_size;
        // 检查对象能否存下term size数据
        // 可以用第0个term的位置来判断是否溢出
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(term_pointer(0))) {
                return nullptr;
            }
        }
        for (length_t index = 0; index < get_list_size(); ++index) {
            *term_size_pointer(index) = 0;
        }
        return this;
    }

    length_t list_t::term_size(length_t index) {
        return *term_size_pointer(index);
    }

    term_t* list_t::term(length_t index) {
        return term_pointer(index);
    }

    void list_t::update_term_size(length_t index) {
        if (index == 0) {
            *term_size_pointer(index) = term(index)->data_size();
        } else {
            *term_size_pointer(index) = term(index)->data_size() + *term_size_pointer(index - 1);
        }
    }

    length_t list_t::data_size() {
        return sizeof(length_t) + sizeof(length_t) * get_list_size() + term_size(get_list_size() - 1);
    }

    std::byte* list_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    std::byte* list_t::tail() {
        return head() + data_size();
    }

    char* list_t::print(char* buffer, char* check_tail) {
        // 在每次写入单个字符前都需要检查
        // 在调用term的print时，传入check_tail并检查返回值是否为nullptr
        if (check_tail != nullptr) {
            if (check_tail <= buffer) {
                return nullptr;
            }
        }
        *(buffer++) = '(';
        for (length_t index = 0; index < get_list_size(); ++index) {
            if (check_tail != nullptr) {
                if (check_tail <= buffer) {
                    return nullptr;
                }
            }
            if (index != 0) {
                *(buffer++) = ' ';
            }
            buffer = term(index)->print(buffer, check_tail);
            if (buffer == nullptr) {
                return nullptr;
            }
        }
        if (check_tail != nullptr) {
            if (check_tail <= buffer) {
                return nullptr;
            }
        }
        *(buffer++) = ')';
        return buffer;
    }

    const char* list_t::scan(const char* buffer, std::byte* check_tail) {
        // 将当前对象的数据暂时作为buffer，以此读取若干个term
        // 在读取的过程中统计list size
        // 在读取完毕后，将当前对象的数据整体向后移动，留出放置list size和各个term size的空间并填写
        ++buffer;
        term_t* term = reinterpret_cast<term_t*>(this);
        length_t list_size = 0;
        while (true) {
            if (*buffer == ')') {
                ++buffer;
                break;
            }
            if (strchr(" \t\r\n", *buffer)) {
                ++buffer;
                continue;
            }
            // 读term的时候需要检查尾指针
            buffer = term->scan(buffer, check_tail);
            if (buffer == nullptr) {
                return nullptr;
            }
            term = reinterpret_cast<term_t*>(term->tail());
            ++list_size;
        }
        length_t offset = sizeof(length_t) + sizeof(length_t) * list_size;
        // 需要将list中的term整体向后移动offset个字节
        // 需要检查目前的尾巴，也就是term加上offset后是否溢出
        if (check_tail != nullptr) {
            if (check_tail < reinterpret_cast<std::byte*>(term) + offset) {
                return nullptr;
            }
        }
        memmove(
            reinterpret_cast<std::byte*>(this) + offset,
            reinterpret_cast<std::byte*>(this),
            reinterpret_cast<std::byte*>(term) - reinterpret_cast<std::byte*>(this)
        );
        // 向后移动数据后，不需要检查前面的空间是否足够
        set_list_size(list_size, nullptr);
        for (length_t index = 0; index < list_size; ++index) {
            update_term_size(index);
        }
        return buffer;
    }
} // namespace ds
