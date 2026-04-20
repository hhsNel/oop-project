#include <iostream>
#include <string>
#include "util/resource-loader.h"
#include "util/resource.h"

#include "res.h"

using namespace util;

extern "C" {
extern unsigned char const _binary_res_hello_world_txt_start[];
extern unsigned char const _binary_res_hello_world_txt_end[];
extern unsigned char const _binary_res_meta_flat_start[];
extern unsigned char const _binary_res_meta_flat_end[];
extern unsigned char const _binary_res_meta_sprite_start[];
extern unsigned char const _binary_res_meta_sprite_end[];
}

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") {
            break;
        } else if (cmd == "lookup") {
            std::string resource_name;
            std::cin >> resource_name;
            
            // Fetch the resource
            resource res = resource_loader::lookup_resource(resource_name);
            
            // We use the resource size as our testable result
            std::cout << "RESULT " << res.size << std::endl;
        } else {
            std::cout << "UNKNOWN_CMD" << std::endl;
        }
    }
    
    return 0;
}
