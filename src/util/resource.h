#pragma once
#define RESOURCE_H

#include <cstddef>

namespace util {
    class resource {
    public:
        resource(void const* begin_ptr, void const* end_ptr);
		virtual ~resource() = default;

        void const* begin;
        void const* end;
        std::size_t size;
    };
}

