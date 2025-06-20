#ifndef CUDS_UTILITY_HXX
#define CUDS_UTILITY_HXX

#include "rule.h++"
#include "term.h++"

#include <cstdio>
#include <cstdlib>
#include <memory>

#define CHECK_CUDA_ERROR(call) \
    { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA error in %s at %s:%d with error code: %d (%s)\n", #call, __FILE__, __LINE__, err, cudaGetErrorString(err)); \
            exit(EXIT_FAILURE); \
        } \
    }

namespace cuds {
    /// @brief 用于给malloc的内存使用的unique_ptr deleter
    struct MallocDeleter {
        void operator()(void* ptr) const;
    };

    /// @brief 用于给malloc的内存使用的unique_ptr
    /// @tparam T 指针的类型
    template<typename T>
    using unique_malloc_ptr = std::unique_ptr<T, MallocDeleter>;

#ifdef __CUDACC__
    /// @brief 用于给cuda malloc的内存使用的unique_ptr deleter
    struct CudaMallocDeleter {
        void operator()(void* ptr) const;
    };

    /// @brief 用于给cuda malloc的内存使用的unique_ptr
    /// @tparam T 指针的类型
    template<typename T>
    using unique_cuda_malloc_ptr = std::unique_ptr<T, CudaMallocDeleter>;

    template<typename T>
    unique_cuda_malloc_ptr<T> copy_host_to_device(unique_malloc_ptr<T>& h_ptr) {
        T* d_ptr;
        CHECK_CUDA_ERROR(cudaMalloc(&d_ptr, h_ptr->data_size()));
        CHECK_CUDA_ERROR(cudaMemcpy(d_ptr, h_ptr.get(), h_ptr->data_size(), cudaMemcpyHostToDevice));
        return unique_cuda_malloc_ptr<cuds::rule_t>(d_ptr);
    }

    template<typename T>
    __global__ void copy_device_to_host_helper_get_data_size(length_t* data_size, T* ptr) {
        *data_size = ptr->data_size();
    }

    template<typename T>
    unique_malloc_ptr<T> copy_device_to_host(unique_cuda_malloc_ptr<T>& d_ptr) {
        length_t* data_size_d;
        CHECK_CUDA_ERROR(cudaMalloc(&data_size_d, sizeof(length_t)));
        copy_device_to_host_helper_get_data_size<<<1, 1>>>(data_size_d, d_ptr.get());
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
        length_t data_size_h;
        CHECK_CUDA_ERROR(cudaMemcpy(&data_size_h, data_size_d, sizeof(length_t), cudaMemcpyDeviceToHost));
        CHECK_CUDA_ERROR(cudaFree(data_size_d));
        T* h_ptr = reinterpret_cast<T*>(malloc(data_size_h));
        CHECK_CUDA_ERROR(cudaMemcpy(h_ptr, d_ptr.get(), data_size_h, cudaMemcpyDeviceToHost));
        return unique_malloc_ptr<cuds::rule_t>(h_ptr);
    }
#endif

    /// @brief 将文本形式的term转化为二进制形式的term
    /// @param text 文本形式的term
    /// @param length 二进制形式的term的数据最大长度
    /// @return 二进制的term, 此指针调用方负责free
    unique_malloc_ptr<term_t> text_to_term(const char* text, length_t length);

    /// @brief 将二进制形式的term转化为文本形式的term
    /// @param term 二进制形式的term
    /// @param length 文本形式的term的文本最大长度
    /// @return 文本形式的term, 此指针调用方负责free
    unique_malloc_ptr<char> term_to_text(term_t* term, length_t length);

    /// @brief 将term使用dictionary进行ground
    /// @param term 待被ground的term
    /// @param dictionary 含有list of pair的term作为的dictionary, pair的两个部分分别是key和value
    /// @param length 结果term的数据最大长度
    /// @return 结果term_t对象的指针, 此指针调用方负责free
    unique_malloc_ptr<term_t> ground(term_t* term, term_t* dictionary, length_t length);

    /// @brief 将term_1匹配term_2
    /// @param term_1 第一个term
    /// @param term_2 第二个term
    /// @param follow_first_for_double_variable 当两个term都是variable时, 是否使用第一个term的名字
    /// @param length 结果term的数据最大长度
    /// @return 结果term_t对象的指针, 此指针调用方负责free
    unique_malloc_ptr<term_t> match(term_t* term_1, term_t* term_2, bool follow_first_for_double_variable, length_t length);

    /// @brief 将文本形式的rule转化为二进制形式的rule
    /// @param text 文本形式的rule
    /// @param length 二进制形式的rule的数据最大长度
    /// @return 二进制的rule, 此指针调用方负责free
    unique_malloc_ptr<rule_t> text_to_rule(const char* text, length_t length);

    /// @brief 将二进制形式的rule转化为文本形式的rule
    /// @param rule 二进制形式的rule
    /// @param length 文本形式的rule的文本最大长度
    /// @return 文本形式的rule, 此指针调用方负责free
    unique_malloc_ptr<char> rule_to_text(rule_t* rule, length_t length);

    /// @brief 将rule使用dictionary进行ground
    /// @param rule 待被ground的rule
    /// @param dictionary 含有list of pair的term作为的dictionary, pair的两个部分分别是key和value
    /// @param length 结果rule的数据最大长度
    /// @return 结果rule_t对象的指针, 此指针调用方负责free
    unique_malloc_ptr<rule_t> ground(rule_t* rule, term_t* dictionary, length_t length);

    /// @brief 将rule使用dictionary进行ground
    /// @param rule 待被ground的rule
    /// @param dictionary 含有list of pair的term的rule作为的dictionary, pair的两个部分分别是key和value
    /// @param length 结果rule的数据最大长度
    /// @return 结果rule_t对象的指针, 此指针调用方负责free
    unique_malloc_ptr<rule_t> ground(rule_t* rule, rule_t* dictionary, length_t length);

    /// @brief 将rule_1匹配rule_2
    /// @param rule_1 第一个rule
    /// @param rule_2 第二个rule
    /// @param length 结果rule的数据最大长度
    /// @return 结果rule_t对象的指针, 此指针调用方负责free
    unique_malloc_ptr<rule_t> match(rule_t* rule_1, rule_t* rule_2, length_t length);
} // namespace cuds

#endif
