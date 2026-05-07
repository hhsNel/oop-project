#pragma once
#define COMPONENTIZED_H

#include <concepts>
#include <meta>
#include <iostream>
#include <string_view>
#include <vector>

namespace util {
	struct component_field {};

	template<std::size_t N>
	struct fixed_string {
		char data[N];
		consteval fixed_string(const char (&s)[N]) {
			for (std::size_t i = 0; i < N; ++i) data[i] = s[i];
		}
		consteval std::string_view view() const { return {data, N - 1}; }
	};

	template<fixed_string field>
	struct component_tag {};

	template <typename T>
	class componentized {
		template <fixed_string field>
		consteval static std::meta::info get_tagged_member() {
			auto ctx = std::meta::access_context::current();
			auto members = std::meta::nonstatic_data_members_of(^^T, ctx);

			for (auto m : members) {
				if (std::meta::identifier_of(m) == field.view()) {
					auto attrs = std::meta::annotations_of(m);
					for (auto attr : attrs) {
						if (std::meta::type_of(attr) == ^^const component_field) {
							return m;
						}
					}
				}
			}
			return {};
		}

	public:
		template<fixed_string field>
		requires (get_tagged_member<field>() != std::meta::info{})
		auto& operator()(component_tag<field>) {
			constexpr auto target = get_tagged_member<field>();
			return static_cast<T *>(this)->[:target:];
		}

		template<fixed_string field>
		requires (get_tagged_member<field>() != std::meta::info{})
		auto const& operator()(component_tag<field>) const {
			constexpr auto target = get_tagged_member<field>();
			return static_cast<const T *>(this)->[:target:];
		}
	};

	/* usage:
	 * class test : public util::componentized<test> {
	 *	 [[=util::component_field{}]] int hp;
	 *	 [[=util::component_field{}]] int max_hp;
	 * 
	 *	 float secret;
	 * 
	 *	 [[=util::component_field{}]] float speed;
	 * 
	 *	 friend class componentized<test>;
	 * };
	 *
	 * int main() {
	 *	 test d(100, 5.5f);
	 * 
	 *	 std::cout << "Initial HP: " << d("hp"_f) << std::endl;
	 *	 d("hp"_f) = 150;
	 *	 std::cout << "Updated HP: " << d("hp"_f) << std::endl;
	 *	 std::cout << "Speed: " << d("speed"_f) << std::endl;
	 * }
	 */ 
}

/* must be global */
template<util::fixed_string field>
consteval util::component_tag<field> operator""_f() {
	return {};
}

