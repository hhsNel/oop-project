#include <iostream>
#include <string>
#include "util/indexed-storage.h"

using namespace util;

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;
    
    indexed_storage<std::string> storage;

    while (std::cin >> cmd) {
        if (cmd == "exit") {
            break;
        } else if (cmd == "add") {
            std::string val;
            std::cin >> val;
            auto id = storage.add(val);
            std::cout << "RESULT " << id << std::endl;
        } else if (cmd == "remove") {
            indexed_storage<std::string>::id_t id;
            std::cin >> id;
            storage.remove(id);
            std::cout << "RESULT OK" << std::endl;
        } else if (cmd == "size") {
            std::cout << "RESULT " << storage.size() << std::endl;
        } else if (cmd == "get") {
            indexed_storage<std::string>::id_t id;
            std::cin >> id;
            if (storage.contains(id)) {
                std::cout << "RESULT " << storage[id] << std::endl;
            } else {
                std::cout << "RESULT NONE" << std::endl;
            }
        } else if (cmd == "iterate") {
            std::cout << "RESULT ";
            if (storage.size() == 0) {
                std::cout << "EMPTY";
            } else {
                bool first = true;
                for (auto it = storage.begin(); it != storage.end(); ++it) {
                    if (!first) std::cout << ",";
                    std::cout << (*it).id << ":" << (*it).value;
                    first = false;
                }
            }
            std::cout << std::endl;
        } else if (cmd == "erase_val") {
            std::string val;
            std::cin >> val;
            for (auto it = storage.begin(); it != storage.end(); ) {
                if ((*it).value == val) {
                    it = storage.erase(it);
                } else {
                    ++it;
                }
            }
            std::cout << "RESULT OK" << std::endl;
        } else {
            std::cout << "RESULT UNKNOWN" << std::endl;
        }
    }
    return 0;
}
