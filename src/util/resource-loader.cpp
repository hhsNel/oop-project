#include "resource-loader.h"

#include <dlfcn.h>
#include <unordered_map>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace util {

    void* resource_loader::lookup_binary(std::string_view resource_name) {
        std::string symbol_name = make_symbol_name(resource_name);

        std::lock_guard<std::mutex> lock(cache_mutex);
        if (auto it = symbol_cache.find(symbol_name); it != symbol_cache.end()) {
            return it->second;
        }

        void* ptr = dlsym(RTLD_DEFAULT, symbol_name.c_str());
        symbol_cache[symbol_name] = ptr;

        return ptr;
    }

    std::unique_ptr<resource> resource_loader::load_file(std::string_view resource_name) const {
        std::filesystem::path file_path = std::filesystem::path("./res") / resource_name;

        if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)) {
            std::ifstream file(file_path, std::ios::binary | std::ios::ate);
            if (file) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<std::byte> buffer(size);
                if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
                    // Changed from make_shared to make_unique[cite: 2]
                    return std::make_unique<file_resource>(std::move(buffer));
                }
            }
        }
        return nullptr;
    }

    resource* resource_loader::lookup_resource_runtime(std::string_view resource_name) {
        std::string res_name_str(resource_name);

        {
            std::lock_guard<std::mutex> lock(cache_mutex);
            if (auto it = resource_cache.find(res_name_str); it != resource_cache.end()) {
                // Return a non-owning pointer from the cache[cite: 2]
                return it->second.get();
            }
        }

        if (std::unique_ptr<resource> file_res = load_file(resource_name)) {
            std::lock_guard<std::mutex> lock(cache_mutex);
            resource* ptr = file_res.get();
            resource_cache[res_name_str] = std::move(file_res);
            return ptr;
        }

        std::string begin_name = res_name_str + "_start";
        std::string end_name = res_name_str + "_end";

        void* begin_ptr = lookup_binary(begin_name);
        void* end_ptr = lookup_binary(end_name);

        if (begin_ptr && end_ptr) {
            auto sym_res = std::make_unique<resource>(begin_ptr, end_ptr);
            std::lock_guard<std::mutex> lock(cache_mutex);
            resource* ptr = sym_res.get();
            resource_cache[res_name_str] = std::move(sym_res);
            return ptr;
        }

        return nullptr;
    }
}

