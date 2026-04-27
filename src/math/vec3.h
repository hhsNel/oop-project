#pragma once
#define VEC3_H

#include "vec2.h"

namespace math {
	class vec3 {
	public:
		float x, y, z;

		constexpr inline vec3(float const X = 0, float const Y = 0, float const Z = 0);
		constexpr inline vec3(vec2 const flat, float const Z = 0);

		__attribute__((always_inline)) constexpr inline vec3 const operator+(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator-(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator-() const;
		__attribute__((always_inline)) constexpr inline vec3 const operator*(float const scalar) const;
		__attribute__((always_inline)) constexpr inline vec3 const operator/(float const scalar) const;
		
		__attribute__((always_inline)) constexpr inline vec3 &operator+=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 &operator-=(vec3 const other);
		__attribute__((always_inline)) constexpr inline vec3 &operator*=(float const scalar);
		__attribute__((always_inline)) constexpr inline vec3 &operator/=(float const scalar);

		__attribute__((always_inline)) constexpr inline bool operator==(vec3 const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(vec3 const other) const;

		static constexpr float dot_product(vec3 const first, vec3 const second);
		static constexpr vec3 const cross_product(vec3 const first, vec3 const second);

		constexpr float len() const;
		constexpr float sqr_len() const;
		constexpr vec3 const normalized() const;
	};
}

constexpr inline
math::vec3::vec3(float const X, float const Y, float const Z) : x(X), y(Y), z(Z) {}

constexpr inline
math::vec3::vec3(vec2 const flat, float const Z) : x(flat.x), y(flat.y), z(Z) {}

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

