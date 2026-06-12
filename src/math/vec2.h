#pragma once
#include "util/componentized.h"

namespace math {
	class vec3;

	class vec2 : public util::componentized<vec2> {
		/* vector components */
		[[=util::ref_component_field{}]] float x;
		/* vector components */
		[[=util::ref_component_field{}]] float y;

	public:

		/* constructor from 2 floats */
		constexpr inline vec2(float const X = 0, float const Y = 0);
		/* constructor from a vec3, discarding z */
		vec2(vec3 const& vec);

		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			operator+(vec2 const other) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			operator-(vec2 const other) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			operator-() const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			operator*(float const scalar) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			operator/(float const scalar) const;
		
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			&operator+=(vec2 const other);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			&operator-=(vec2 const other);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			&operator*=(float const scalar);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec2
			&operator/=(float const scalar);

		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator==(vec2 const other) const;
		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator!=(vec2 const other) const;

		/* dot product of 2 vec2s */
		static float dot_product(vec2 const first, vec2 const second);
		/* cross product of 2 vec2s */
		static float cross_product(vec2 const first, vec2 const second);

		/* vector length */
		float len() const;
		/* vector square length */
		float sqr_len() const;
		/* normalized vector */
		vec2 const normalized() const;
		/* a vector perpendicular to a given one */
		vec2 const perpendicular() const;
		/* vector argument */
		float angle() const;

		/* angle between 2 vectors */
		static float angle_between(vec2 const first, vec2 const second);
		/* vector rotation */
		static vec2 const rotate(vec2 const vec, float const angle);
		/* optimized vector rotation */
		static vec2 const rotate_with_known_trig(vec2 const vec,
			float const cos, float const sin);

		friend util::componentized<vec2>;
	};
}

/* constexpr inline definitions */
constexpr inline
math::vec2::vec2(float const X, float const Y) : x(X), y(Y) {}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator+(vec2 const other) const {
	return vec2(x + other.x, y + other.y);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator-(vec2 const other) const {
	return vec2(x - other.x, y - other.y);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator-() const {
	return vec2(-x, -y);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator*(float const scalar) const {
	return vec2(x * scalar, y * scalar);
}

__attribute__((always_inline)) constexpr inline math::vec2
math::vec2::operator/(float const scalar) const {
	return vec2(x / scalar, y / scalar);
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator+=(vec2 const other) {
	x += other.x;
	y += other.y;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator-=(vec2 const other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator*=(float const scalar) {
	x *= scalar;
	y *= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec2&
math::vec2::operator/=(float const scalar) {
	x /= scalar;
	y /= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline bool
math::vec2::operator==(vec2 const other) const {
	return x == other.x && y == other.y;
}

__attribute__((always_inline)) constexpr inline bool
math::vec2::operator!=(vec2 const other) const {
	return !(*this == other);
}

