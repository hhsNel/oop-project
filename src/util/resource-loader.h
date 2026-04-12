#pragma once
#define RESOURCE_LOADER_H

#include <string>
#include <string_view>

#include "resource.h"

namespace util {
	class resource_loader {
		static constexpr std::string const make_symbol_name(std::string_view const resource);
		static consteval void *const get_by_symbol_reflection(std::string_view const symbol_name);
		static void *const lookup_binary(std::string_view const resource_name);
	public:
		static resource const lookup_resource(std::string_view const resource_name);
	}
}

