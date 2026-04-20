#pragma once

#include <cstddef>

namespace util {
    class resource {
    public:
        resource(void const* begin_ptr, void const* end_ptr);

        void const* begin;
        void const* end;
        std::size_t size;
    };
}
