#pragma once
#define VEC2_H

#include "fxpt.h"

namespace math {
	class vec3;

	class vec2 {
	public:
		fxpt a, b;

		constexpr inline vec2(fxpt const A = 0, fxpt const B = 0);
		vec2(vec3 const& vec);

		__attribute__((always_inline)) constexpr inline vec2 operator+(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 operator-(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 operator-() const;
		__attribute__((always_inline)) constexpr inline vec2 operator*(fxpt const scalar) const;
		__attribute__((always_inline)) constexpr inline vec2 operator/(fxpt const scalar) const;
		
		__attribute__((always_inline)) constexpr inline vec2 &operator+=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 &operator-=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 &operator*=(fxpt const scalar);
		__attribute__((always_inline)) constexpr inline vec2 &operator/=(fxpt const scalar);

		__attribute__((always_inline)) constexpr inline bool operator==(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(vec2 const other) const;

		static fxpt const dot_product(vec2 const first, vec2 const second);

		fxpt const len() const;
		fxpt const sqr_len() const;
		vec2 const normalized() const;
		vec2 const perpendicular() const;
		fxpt const angle() const;

		static fxpt const angle_between(vec2 const first, vec2 const second);
		static vec2 const rotate(vec2 const vec, fxpt const angle);
	};
}

constexpr inline
math::vec2::vec2(fxpt const A, fxpt const B) : a(A), b(B) {}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator+(vec2 const other) const {
	return vec2(a + other.a, b + other.b);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator-(vec2 const other) const {
	return vec2(a - other.a, b - other.b);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator-() const {
	return vec2(-a, -b);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator*(fxpt const scalar) const {
	return vec2(a * scalar, b * scalar);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator/(fxpt const scalar) const {
	return vec2(a / scalar, b / scalar);
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator+=(vec2 const other) {
	a += other.a;
	b += other.b;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator-=(vec2 const other) {
	a -= other.a;
	b -= other.b;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator*=(fxpt const scalar) {
	a *= scalar;
	b *= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator/=(fxpt const scalar) {
	a /= scalar;
	b /= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline bool
math::vec2::operator==(vec2 const other) const {
	return a == other.a && b == other.b;
}

__attribute__((always_inline)) constexpr inline bool
math::vec2::operator!=(vec2 const other) const {
	return !(*this == other);
}
