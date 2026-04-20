#include "resource.h"

namespace util {
    resource::resource(void const* begin_ptr, void const* end_ptr) :
        begin(begin_ptr),
        end(end_ptr),
        size(static_cast<std::size_t>(static_cast<std::byte const *>(end_ptr) - static_cast<std::byte const *>(begin_ptr)))
    {}
}
