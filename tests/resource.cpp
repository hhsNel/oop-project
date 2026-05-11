#include <iostream>
#include <string>
#include "util/resource-loader.h"
#include "util/resource.h"

using namespace util;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;
	util::resource_loader rl;

    while (std::cin >> cmd) {
        if (cmd == "exit") {
            break;
        } else if (cmd == "lookup") {
            std::string resource_name;
            std::cin >> resource_name;
            
            resource &res = *rl.lookup_resource(resource_name);
            
            std::cout << "RESULT " << res("size"_f) << std::endl;
        } else if (cmd == "dump") {
            std::string resource_name;
            std::cin >> resource_name;
            
            resource &res = *rl.lookup_resource(resource_name);
            
            std::cout << "RESULT ";
			std::cout.write((char *)res("beginning"_f), res("size"_f));
			std::cout << std::endl;
		} else {
            std::cout << "UNKNOWN_CMD" << std::endl;
        }
    }
    
    return 0;
}
