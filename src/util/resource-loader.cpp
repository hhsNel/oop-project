#include "resource-loader.h"

#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <vector>

namespace util {
	void* resource_loader::lookup_binary(std::string_view resource_name) {
		std::string symbol_name = make_symbol_name(resource_name);

		{
			std::shared_lock<std::shared_mutex> rlock(cache_mutex);
			if (auto it = symbol_cache.find(symbol_name); it != symbol_cache.end()) {
				return it->second;
			}
		}

		void* ptr = dlsym(RTLD_DEFAULT, symbol_name.c_str());

		{
			std::unique_lock<std::shared_mutex> wlock(cache_mutex);
			symbol_cache.emplace(symbol_name, ptr);
		}

		return ptr;
	}

	std::unique_ptr<resource> resource_loader::load_file(std::string_view resource_name) const {
		std::filesystem::path file_path = std::filesystem::path("./res") / resource_name;

		if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)) {
			std::ifstream file(file_path, std::ios::binary | std::ios::ate);
			if (file) {
				std::streamsize size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::vector<std::byte> buffer(static_cast<std::size_t>(size));
				if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
					return std::make_unique<file_resource>(std::move(buffer));
				}
			}
		}
		return nullptr;
	}

	resource* resource_loader::lookup_resource_runtime(std::string_view resource_name) {
		std::string res_name_str(resource_name);

		{
			std::shared_lock<std::shared_mutex> rlock(cache_mutex);
			if (auto it = resource_cache.find(res_name_str); it != resource_cache.end()) {
				return it->second.get();
			}
		}

		if (std::unique_ptr<resource> file_res = load_file(resource_name)) {
			std::unique_lock<std::shared_mutex> wlock(cache_mutex);
			resource* ptr = file_res.get();
			resource_cache.emplace(res_name_str, std::move(file_res));
			return ptr;
		}

		std::string begin_name = res_name_str + "_start";
		std::string end_name   = res_name_str + "_end";

		void* begin_ptr = lookup_binary(begin_name);
		void* end_ptr   = lookup_binary(end_name);

		if (begin_ptr && end_ptr) {
			auto sym_res = std::make_unique<binary_resource>(begin_ptr, end_ptr);
			std::unique_lock<std::shared_mutex> wlock(cache_mutex);
			resource* ptr = sym_res.get();
			resource_cache.emplace(res_name_str, std::move(sym_res));
			return ptr;
		}

		return nullptr;
	}

}
