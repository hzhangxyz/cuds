#include "item.h++"

namespace cuds {
    CUDA_HOST_DEVICE string_t* item_t::name() {
        return reinterpret_cast<string_t*>(this);
    }

    CUDA_HOST_DEVICE length_t item_t::data_size() {
        return name()->data_size();
    }

    CUDA_HOST_DEVICE std::byte* item_t::head() {
        return reinterpret_cast<std::byte*>(this);
    }

    CUDA_HOST_DEVICE std::byte* item_t::tail() {
        return head() + data_size();
    }

    CUDA_HOST_DEVICE char* item_t::print(char* buffer) {
        return name()->print(buffer);
    }

    CUDA_HOST_DEVICE const char* item_t::scan(const char* buffer) {
        return name()->scan(buffer);
    }
} // namespace cuds
