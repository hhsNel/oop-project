#pragma once
#define VEC2_H

#include "fxpt.h"

namespace math {
	class vec3;

	class vec2 {
	public:
		fxpt a, b;

		constexpr inline vec2(fxpt const A = 0, fxpt const B = 0);
		constexpr inline vec2(vec3 const vec);

		__attribute__((always_inline)) constexpr inline vec2 const operator+(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 const operator-(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 const operator*(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec2 const operator/(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec2 const operator+=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 const operator-=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 const operator*=(fxpt const scalar);
		__attribute__((always_inline)) constexpr inline vec2 const operator/=(fxpt const scalar);

		static constexpr fxpt const dot_product(vec2 const first, vec2 const second);

		constexpr fxpt const len() const;
		constexpr fxpt const sqr_len() const;
		constexpr vec2 const normalized() const;
		constexpr vec2 const perpendicular() const;
		constexpr fxpt const angle() const;

		static constexpr fxpt const angle_between(vec2 const first, vec2 const second);
		static constexpr vec2 const rotate(vec2 const vec, fxpt const angle);
	};
}

