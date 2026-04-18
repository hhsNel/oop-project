#include "resource.h"

namespace util {
    resource::resource(const std::byte* begin_ptr, const std::byte* end_ptr) :
        begin(begin_ptr),
        end(end_ptr),
        size(static_cast<std::size_t>(end_ptr - begin_ptr))
    {}
}