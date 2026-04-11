#include "resource-loader.h"

#include <meta>
#include <cctype>
#include <type_traits>
#include <dlfcn.h>

constexpr std::string
util::resource_loader::make_symbol_name(std::string_view resource) const {
	std::string sym = "_binary_res_";

	for (char ch : resource) {
		unsigned char c = static_cast<unsigned char>(ch);
		if (std::isalnum(c)) {
			sym += ch;
		} else {
			sym += '_';
		}
	}
	return sym;
}

consteval void *
util::resource_loader::get_by_symbol_reflection(std::string_view symbol_name) const {
	auto ctx = std::meta::access_context::current();

	for (std::meta::info mem : std::meta::members_of(^^::, ctx)) {
		if (std::meta::has_identifier(mem) && std::meta::identifier_of(mem) == symbol_name) {

			return static_cast<void*>( & [: mem :] );
		}
	}
	return nullptr;
}

void *
util::resource_loader::lookup_binary_resource(std::string_view resource_name) const {
    std::string symbol_name = util::resource_loader::make_symbol_name(resource_name);

    if (std::is_constant_evaluated()) {
        return util::resource_loader::get_by_symbol_reflection(symbol_name);
    } else {
        return dlsym(RTLD_DEFAULT, symbol_name.c_str());
    }
}

