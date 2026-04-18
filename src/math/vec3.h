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
		__attribute__((always_inline)) constexpr inline vec3 const operator-() const;
		__attribute__((always_inline)) constexpr inline vec3 const operator*(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator/(fxpt const scalar) const;
		
		__attribute__((always_inline)) constexpr inline vec3 &operator+=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 &operator-=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 &operator*=(fxpt const scalar);
		__attribute__((always_inline)) constexpr inline vec3 &operator/=(fxpt const scalar);

		__attribute__((always_inline)) constexpr inline bool operator==(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(vec3 const other) const;

		static constexpr fxpt const dot_product(vec3 const first, vec3 const second);
		static constexpr vec3 const cross_product(vec3 const first, vec3 const second);

		constexpr fxpt const len() const;
		constexpr fxpt const sqr_len() const;
		constexpr vec3 const normalized() const;
	};
}

constexpr inline
math::vec3::vec3(fxpt const A, fxpt const B, fxpt const C) : a(A), b(B), c(C) {}

constexpr inline
math::vec3::vec3(vec2 const flat, fxpt const z) : a(flat.a), b(flat.b), c(z) {}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator+(vec3 const other) const {
	return vec3(a + other.a, b + other.b, c + other.c);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator-(vec3 const other) const {
	return vec3(a - other.a, b - other.b, c - other.c);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator-() const {
	return vec3(-a, -b, -c);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator*(fxpt const scalar) const {
	return vec3(a * scalar, b * scalar, c * scalar);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator/(fxpt const scalar) const {
	return vec3(a / scalar, b / scalar, c / scalar);
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator+=(vec3 const other) {
	a += other.a; b += other.b; c += other.c;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator-=(vec3 const other) {
	a -= other.a; b -= other.b; c -= other.c;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator*=(fxpt const scalar) {
	a *= scalar; b *= scalar; c *= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator/=(fxpt const scalar) {
	a /= scalar; b /= scalar; c /= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline bool
math::vec3::operator==(vec3 const other) const {
	return a == other.a && b == other.b && c == other.c;
}

__attribute__((always_inline)) constexpr inline bool
math::vec3::operator!=(vec3 const other) const {
	return !(*this == other);
}
