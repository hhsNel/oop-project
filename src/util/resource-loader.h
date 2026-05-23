#pragma once

/* auto-generated file with all declarations */
#include "res.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <memory>

#if __has_include(<meta>)
#include <meta>
#endif

#include "resource.h"
#include "binary-resource.h"
#include "file-resource.h"

namespace util {
	class resource_loader {
		std::unordered_map<std::string, void*> symbol_cache;
		std::unordered_map<std::string, std::unique_ptr<resource>> resource_cache;
		mutable std::shared_mutex cache_mutex;

		void* lookup_binary(std::string_view resource_name);
		std::unique_ptr<resource> load_file(std::string_view resource_name) const;
		resource* lookup_resource_runtime(std::string_view resource_name);

		constexpr std::string make_symbol_name(std::string_view resource_name) const;

#if __has_include(<meta>)
		static consteval std::size_t count_res_members();

		template <std::size_t N>
		struct info_array {
			std::meta::info data[N > 0 ? N : 1];
		};

		template <std::size_t N>
		static consteval info_array<N> get_res_members_array();

		constexpr resource const* get_by_symbol_reflection(std::string_view symbol_name) const;
#endif

	public:
		resource_loader()  = default;
		~resource_loader() = default;

		resource_loader(resource_loader const&)			= delete;
		resource_loader& operator=(resource_loader const&) = delete;

		constexpr resource* lookup_resource(std::string_view resource_name);
	};

	constexpr std::string resource_loader::make_symbol_name(std::string_view resource_name) const {
		std::string sym = "_binary_res_";
		sym.reserve(sym.size() + resource_name.size());

		for (char ch : resource_name) {
			unsigned char c = static_cast<unsigned char>(ch);
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
				sym += ch;
			} else {
				sym += '_';
			}
		}
		return sym;
	}

#if __has_include(<meta>)
	consteval std::size_t resource_loader::count_res_members() {
		std::size_t count = 0;
		for (auto m : std::meta::members_of(^^res_symbols, std::meta::access_context::current())) {
			if (std::meta::has_identifier(m)) {
				if (std::string_view(std::meta::identifier_of(m)).starts_with("_binary_res_")) {
					++count;
				}
			}
		}
		return count;
	}

	template <std::size_t N>
	consteval resource_loader::info_array<N> resource_loader::get_res_members_array() {
		resource_loader::info_array<N> arr{};
		std::size_t idx = 0;
		for (auto m : std::meta::members_of(^^res_symbols, std::meta::access_context::current())) {
			if (std::meta::has_identifier(m)) {
				if (std::string_view(std::meta::identifier_of(m)).starts_with("_binary_res_")) {
					if (idx < N) {
						arr.data[idx++] = m;
					}
				}
			}
		}
		return arr;
	}

	constexpr resource const* resource_loader::get_by_symbol_reflection(std::string_view symbol_name) const {
		if consteval {
#if __has_include(<meta>)
			static constexpr std::size_t num_members = count_res_members();
			static constexpr auto members_arr = get_res_members_array<num_members>();

			if (!symbol_name.ends_with("_start")) {
				return nullptr;
			}
			std::string end_name(symbol_name.substr(0, symbol_name.size() - 6));
			end_name += "_end";

			unsigned char const* begin_sym = nullptr;
			unsigned char const* end_sym   = nullptr;

			/* TODO: work around a gcc 16.1.1 20260430 bug */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
			template for (constexpr auto m : members_arr.data) {
				if constexpr (std::meta::has_identifier(m)) {
					constexpr std::string_view name = std::meta::identifier_of(m);
					if (name == symbol_name) {
						begin_sym = [: m :];
					} else if (name == end_name) {
						end_sym = [: m :];
					}
				}
			}
#pragma GCC diagnostic pop

			if (begin_sym && end_sym) {
				return std::define_static_object(
					binary_resource(
						static_cast<void const*>(begin_sym),
						static_cast<void const*>(end_sym)));
			}
#endif
			return nullptr;
		} else {
			/* not consteval, abort */
			return nullptr;
		}
	}

	constexpr resource* resource_loader::lookup_resource(std::string_view resource_name) {
		if consteval {
			std::string begin_name = make_symbol_name(std::string(resource_name) + "_start");

			return const_cast<resource*>(get_by_symbol_reflection(begin_name));
		} else {
			return lookup_resource_runtime(resource_name);
		}
	}
#endif

}
