#pragma once

#include <string>
#include <string_view>
#include "resource.h"

namespace util {
    class resource_loader {
    private:
        static std::string make_symbol_name(std::string_view resource);
        static void* get_by_symbol_reflection(std::string_view symbol_name);
        static void* lookup_binary(std::string_view resource_name);
        
    public:
        static resource lookup_resource(std::string_view resource_name);
    };
}
