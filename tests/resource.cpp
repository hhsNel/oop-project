#include <iostream>
#include <string>
#include "util/resource-loader.h"
#include "util/resource.h"

using namespace util;

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
        } else if (cmd == "dump") {
            std::string resource_name;
            std::cin >> resource_name;
            
            resource res = resource_loader::lookup_resource(resource_name);
            
            std::cout << "RESULT ";
			std::cout.write((char *)res.begin, res.size);
			std::cout << std::endl;
		} else {
            std::cout << "UNKNOWN_CMD" << std::endl;
        }
    }
    
    return 0;
}
