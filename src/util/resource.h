#pragma once
#define RESOURCE_H

#include <cstddef>

namespace util {
	class resource {
	public:
		std::byte *begin;
		std::byte *end;
		std::size_t size;
		resource();
		resource(std::byte *begin_ptr, std::byte *end_ptr);
	};
}

