#include "binary-resource.h"

namespace util {
	void const* binary_resource::operator()(component_tag<"beginning">) const {
		return begin_ptr;
	}

	void const* binary_resource::operator()(component_tag<"end">) const {
		return end_ptr;
	}

	std::size_t binary_resource::operator()(component_tag<"size">) const {
		return byte_size;
	}

	binary_resource::binary_resource(void const* begin, void const* end) :
		begin_ptr(begin),
		end_ptr(end),
		byte_size(static_cast<std::size_t>(static_cast<std::byte const*>(end) - static_cast<std::byte const*>(begin)))
	{}
}

