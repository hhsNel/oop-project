#pragma once
#define VEC3_H

#include "fxpt.h"
#include "vec2.h"

namespace math {
	class vec3 {
	public:
		fxpt a, b, c;

		constexpr inline vec3(fxpt const A = 0, fxpt const B = 0, fxpt const C = 0);
		constexpr inline vec3(vec2 const flat, fxpt const z = 0);

		__attribute__((always_inline)) constexpr inline vec3 const operator+(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator-(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator*(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator/(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator+=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 const operator-=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 const operator*=(fxpt const scalar);
		__attribute__((always_inline)) constexpr inline vec3 const operator/=(fxpt const scalar);

		static constexpr fxpt const dot_product(vec3 const first, vec3 const second);
		static constexpr vec3 const cross_product(vec3 const first, vec3 const second);

		constexpr fxpt const len() const;
		constexpr fxpt const sqr_len() const;
		constexpr vec3 const normalized() const;
	};
}

