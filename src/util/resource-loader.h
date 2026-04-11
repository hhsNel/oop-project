#pragma once
#define RESOURCE_LOADER_H

#include <string>
#include <string_view>

namespace util {
	class resource_loader {
		constexpr std::string make_symbol_name(std::string_view resource) const;
		consteval void* get_by_symbol_reflection(std::string_view symbol_name) const;
	public:
		void* lookup_binary_resource(std::string_view resource_name) const;
	}
}

