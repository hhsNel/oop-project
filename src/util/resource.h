#pragma once
#define RESOURCE_H

#include <cstddef>

namespace util {
	class resource_loader;
	class resource {
		resource(std::byte const *const begin_ptr, std::byte const *const end_ptr);

		friend resource_loader;
	public:
		std::byte const *const begin;
		std::byte const *const end;
		std::size_t const size;
	};
}

