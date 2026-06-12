#pragma once
#define COMPONENTIZED_H

#include <concepts>
#include <meta>
#include <iostream>
#include <string_view>
#include <vector>

namespace util {
	/* c++26 meta annotation */
	struct component_field {};
	/* c++26 meta annotation */
	struct ref_component_field {};

	/* char array for any string size */
	template<std::size_t N>
	struct fixed_string {
		char data[N];
		/* constructor */
		consteval fixed_string(char const (&s)[N]) {
			for (std::size_t i = 0; i < N; ++i) data[i] = s[i];
		}
		/* convert to std::string_view explicitly */
		consteval std::string_view view() const { return {data, N - 1}; }
	};

	/* template metaprogramming argument struct */
	template<fixed_string field>
	struct component_tag {};

	template <typename T>
	class componentized {
		/* get a std::meta::info of a field with annotation, or {} if not found */
		template <fixed_string field, typename A>
		consteval static std::meta::info get_tagged_member() {
			auto ctx = std::meta::access_context::current();
			auto members = std::meta::nonstatic_data_members_of(^^T, ctx);

			for (auto m : members) {
				if (std::meta::identifier_of(m) == field.view()) {
					auto attrs = std::meta::annotations_of(m);
					for (auto attr : attrs) {
						if (std::meta::type_of(attr) == ^^A) {
							return m;
						}
					}
				}
			}
			return {};
		}

	public:
		/* auto& operator() for ref_component_field only */
		template<fixed_string field>
		requires (get_tagged_member<field, ref_component_field const>() != std::meta::info{})
		auto& operator()(component_tag<field>) {
			constexpr auto target = get_tagged_member<field, ref_component_field const>();
			return static_cast<T *>(this)->[:target:];
		}

		/* auto const& operator() for ref_component_field or component_field */
		template<fixed_string field>
		requires
			( (get_tagged_member<field, component_field const>()	 != std::meta::info{}) ||
			  (get_tagged_member<field, ref_component_field const>() != std::meta::info{}) )
		auto const& operator()(component_tag<field>) const {
			constexpr auto target = []{
				auto tgt = get_tagged_member<field, component_field const>();
				if(tgt != std::meta::info{}) return tgt;
				return get_tagged_member<field, ref_component_field const>();
			}();
			return static_cast<T const *>(this)->[:target:];
		}
	};

	/* usage:
	 * class test : public util::componentized<test> {
	 *	 [[=util::ref_component_field{}]] int hp;
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

/* operator""_f for the "hp"_f notation */
/* must be global */
template<util::fixed_string field>
consteval util::component_tag<field> operator""_f() {
	return {};
}

