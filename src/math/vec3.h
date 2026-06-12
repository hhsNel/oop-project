#pragma once

#include "util/componentized.h"
#include "vec2.h"

namespace math {
	class vec3 : public util::componentized<vec3> {
		/* vector components */
		[[=util::ref_component_field{}]] float x;
		/* vector components */
		[[=util::ref_component_field{}]] float y;
		/* vector components */
		[[=util::ref_component_field{}]] float z;
	public:

		/* constructor from 3 floats */
		constexpr inline vec3(float const X = 0, float const Y = 0,
			float const Z = 0);
		/* constructor from a vec2 and height */
		constexpr inline vec3(vec2 const flat, float const Z = 0);

		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3 const
			operator+(vec3 const other) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3 const
			operator-(vec3 const other) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3 const
			operator-() const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3 const
			operator*(float const scalar) const;
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3 const
			operator/(float const scalar) const;
		
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3
			&operator+=(vec3 const other);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3
			&operator-=(vec3 const other);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3
			&operator*=(float const scalar);
		/* algebraic operators */
		__attribute__((always_inline)) constexpr inline vec3
			&operator/=(float const scalar);

		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator==(vec3 const other) const;
		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator!=(vec3 const other) const;

		/* dot product of 2 vec3s */
		static constexpr float dot_product(vec3 const first, vec3 const second);
		/* cross product of 2 vec3s */
		static constexpr vec3 const cross_product(vec3 const first, vec3 const second);

		/* vector length */
		constexpr float len() const;
		/* vector square length */
		constexpr float sqr_len() const;
		/* normalized vector */
		constexpr vec3 const normalized() const;

		friend util::componentized<vec3>;
	};
}

/* constexpr inline definitions */
constexpr inline
math::vec3::vec3(float const X, float const Y, float const Z) : x(X), y(Y), z(Z) {}

constexpr inline
math::vec3::vec3(vec2 const flat, float const Z) : x(flat("x"_f)), y(flat("y"_f)), z(Z) {}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator+(vec3 const other) const {
	return vec3(x + other.x, y + other.y, z + other.z);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator-(vec3 const other) const {
	return vec3(x - other.x, y - other.y, z - other.z);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator-() const {
	return vec3(-x, -y, -z);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator*(float const scalar) const {
	return vec3(x * scalar, y * scalar, z * scalar);
}

__attribute__((always_inline)) constexpr inline math::vec3 const
math::vec3::operator/(float const scalar) const {
	return vec3(x / scalar, y / scalar, z / scalar);
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator+=(vec3 const other) {
	x += other.x; y += other.y; z += other.z;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator-=(vec3 const other) {
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator*=(float const scalar) {
	x *= scalar; y *= scalar; z *= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::vec3 &
math::vec3::operator/=(float const scalar) {
	x /= scalar; y /= scalar; z /= scalar;
	return *this;
}

__attribute__((always_inline)) constexpr inline bool
math::vec3::operator==(vec3 const other) const {
	return x == other.x && y == other.y && z == other.z;
}

__attribute__((always_inline)) constexpr inline bool
math::vec3::operator!=(vec3 const other) const {
	return !(*this == other);
}

constexpr float math::vec3::dot_product(vec3 const first, vec3 const second) {
	return first.x * second.x + first.y * second.y + first.z * second.z;
}

constexpr math::vec3 const math::vec3::cross_product(vec3 const first, vec3 const second) {
	return vec3(
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	);
}

