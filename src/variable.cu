#include "variable.h++"

namespace cuds {
    CUDA_HOST_DEVICE string_t* variable_t::name() {
        return reinterpret_cast<string_t*>(this);
    }

    CUDA_HOST_DEVICE length_t variable_t::data_size() {
        return name()->data_size();
    }

    CUDA_HOST_DEVICE std::byte* variable_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    CUDA_HOST_DEVICE std::byte* variable_t::tail() {
        return head() + data_size();
    }

    CUDA_HOST_DEVICE char* variable_t::print(char* buffer) {
        *(buffer++) = '\'';
        return name()->print(buffer);
    }

    CUDA_HOST_DEVICE const char* variable_t::scan(const char* buffer) {
        return name()->scan(buffer + 1);
    }
} // namespace cuds
