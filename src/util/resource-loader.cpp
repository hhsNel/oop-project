#include "resource-loader.h"

#include <meta>
#include <cctype>
#include <type_traits>
#include <dlfcn.h>

static constexpr std::string
util::resource_loader::make_symbol_name(std::string_view resource) {
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

static consteval void *
util::resource_loader::get_by_symbol_reflection(std::string_view symbol_name) {
	auto ctx = std::meta::access_context::current();

	for (std::meta::info mem : std::meta::members_of(^^::, ctx)) {
		if (std::meta::has_identifier(mem) && std::meta::identifier_of(mem) == symbol_name) {

			return static_cast<void*>( & [: mem :] );
		}
	}
	return nullptr;
}

static void *
util::resource_loader::lookup_binary(std::string_view resource_name) {
    std::string symbol_name = util::resource_loader::make_symbol_name(resource_name);

    if (std::is_constant_evaluated()) {
        return util::resource_loader::get_by_symbol_reflection(symbol_name);
    } else {
        return dlsym(RTLD_DEFAULT, symbol_name.c_str());
    }
}

static resource const
util::resource_loader::lookup_resource(std::string_view const resource_name) {
	return resource(
			lookup_binary("_binary_res_" + std::string(sv.begin(), sv.end()) + "_start"),
			lookup_binary("_binary_res_" + std::string(sv.begin(), sv.end()) + "_end")
			);
}

