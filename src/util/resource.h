#pragma once

#include <cstddef>

namespace util {
    class resource {
    public:
        resource(const std::byte* begin_ptr, const std::byte* end_ptr);

        const std::byte* begin;
        const std::byte* end;
        std::size_t size;
    };
}
