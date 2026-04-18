#include "resource-loader.h"

#include <cctype>
#include <dlfcn.h>
#include <unordered_map>
#include <mutex>

namespace util {

    std::string resource_loader::make_symbol_name(std::string_view resource) {
        std::string sym = "_binary_res_";
        sym.reserve(sym.size() + resource.size());
        
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

    void* resource_loader::get_by_symbol_reflection(std::string_view symbol_name) {
        return nullptr;

		/* dummy to make the compiler shut up, at least until we implement something that works */
		return (void *)std::string(symbol_name).c_str();
    }

    void* resource_loader::lookup_binary(std::string_view resource_name) {
        static std::unordered_map<std::string, void*> symbol_cache;
        static std::mutex cache_mutex;

        std::string symbol_name = make_symbol_name(resource_name);

        std::lock_guard<std::mutex> lock(cache_mutex);
        if (auto it = symbol_cache.find(symbol_name); it != symbol_cache.end()) {
            return it->second;
        }

        void* ptr = dlsym(RTLD_DEFAULT, symbol_name.c_str());
        symbol_cache[symbol_name] = ptr;
        
        return ptr;
    }

    resource resource_loader::lookup_resource(std::string_view resource_name) {
        std::string begin_name = std::string(resource_name) + "_start";
        std::string end_name = std::string(resource_name) + "_end";

        void* begin_ptr = lookup_binary(begin_name);
        void* end_ptr = lookup_binary(end_name);

        return resource(static_cast<const std::byte*>(begin_ptr), 
                        static_cast<const std::byte*>(end_ptr));
    }
}
