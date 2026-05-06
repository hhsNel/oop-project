#pragma once

/* auto-generated file with all declarations */
#include "res.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <mutex>
#include <memory>

#if __has_include(<meta>)
#include <meta>
#endif

#include "resource.h"
#include "file-resource.h"

namespace util {
    class resource_loader {
        std::unordered_map<std::string, void*> symbol_cache;
        std::unordered_map<std::string, std::unique_ptr<resource>> resource_cache;
        std::mutex cache_mutex;

        void* lookup_binary(std::string_view resource_name);
        std::unique_ptr<resource> load_file(std::string_view resource_name) const;
        resource* lookup_resource_runtime(std::string_view resource_name);

        constexpr std::string make_symbol_name(std::string_view resource) const;

		static consteval std::size_t count_res_members();
		template <std::size_t N>
        struct info_array {
            std::meta::info data[N > 0 ? N : 1];
        };
		template <std::size_t N>
        static consteval info_array<N> get_res_members_array();
        constexpr void* get_by_symbol_reflection(std::string_view symbol_name) const;
    public:
        resource_loader() = default;
        ~resource_loader() = default;

        resource_loader(const resource_loader&) = delete;
        resource_loader& operator=(const resource_loader&) = delete;

        constexpr resource* lookup_resource(std::string_view resource_name);
    };

    constexpr std::string resource_loader::make_symbol_name(std::string_view resource) const {
        std::string sym = "_binary_res_";
        sym.reserve(sym.size() + resource.size());

        for (char ch : resource) {
            unsigned char c = static_cast<unsigned char>(ch);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                sym += ch;
            } else {
                sym += '_';
            }
        }
        return sym;
    }

	consteval std::size_t resource_loader::count_res_members() {
		std::size_t count = 0;
#if __has_include(<meta>)
		for (auto m : std::meta::members_of(^^::, std::meta::access_context::current())) {
			if (std::meta::has_identifier(m)) {
				if (std::string_view(std::meta::identifier_of(m)).starts_with("_binary_res_")) {
					count++;
				}
			}
		}
#endif
		return count;
	}

	template <std::size_t N>
	consteval resource_loader::info_array<N> resource_loader::get_res_members_array() {
		resource_loader::info_array<N> arr{};
#if __has_include(<meta>)
		std::size_t idx = 0;
		for (auto m : std::meta::members_of(^^::, std::meta::access_context::current())) {
			if (std::meta::has_identifier(m)) {
				if (std::string_view(std::meta::identifier_of(m)).starts_with("_binary_res_")) {
					if (idx < N) {
						arr.data[idx++] = m;
					}
				}
			}
		}
#endif
		return arr;
	}

    constexpr void* resource_loader::get_by_symbol_reflection(std::string_view symbol_name) const {
        if consteval {
            void* found_ptr = nullptr;

#if __has_include(<meta>)
			static constexpr std::size_t num_members = count_res_members();
            static constexpr auto members_arr = get_res_members_array<num_members>();

			/* TODO: work around a gcc 16.1.1 20260430 bug */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
            template for (constexpr auto m : members_arr.data) {
                if constexpr (std::meta::has_identifier(m)) {
                    constexpr std::string_view name = std::meta::identifier_of(m);

					if (name == symbol_name) {
						found_ptr = const_cast<void*>(static_cast<const void*>(&[: m :]));
					}
                }
            }
#pragma GCC diagnostic pop

#endif
            return found_ptr;
        } else {
            /* not consteval; abort */
            return nullptr;
        }
    }

    constexpr resource* resource_loader::lookup_resource(std::string_view resource_name) {
        if consteval {
            std::string res_name_str(resource_name);
            std::string begin_name = make_symbol_name(res_name_str + "_start");
            std::string end_name = make_symbol_name(res_name_str + "_end");

            void* begin_ptr = get_by_symbol_reflection(begin_name);
            void* end_ptr = get_by_symbol_reflection(end_name);

            if (begin_ptr && end_ptr) {
                return new resource(begin_ptr, end_ptr);
            }
            return nullptr;
        } else {
            return lookup_resource_runtime(resource_name);
        }
    }
}

