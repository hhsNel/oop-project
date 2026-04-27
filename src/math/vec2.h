#pragma once
#define VEC2_H

namespace math {
	class vec3;

	class vec2 {
	public:
		float x, y;

		constexpr inline vec2(float const X = 0, float const Y = 0);
		vec2(vec3 const& vec);

		__attribute__((always_inline)) constexpr inline vec2 operator+(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 operator-(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline vec2 operator-() const;
		__attribute__((always_inline)) constexpr inline vec2 operator*(float const scalar) const;
		__attribute__((always_inline)) constexpr inline vec2 operator/(float const scalar) const;
		
		__attribute__((always_inline)) constexpr inline vec2 &operator+=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 &operator-=(vec2 const other);
		__attribute__((always_inline)) constexpr inline vec2 &operator*=(float const scalar);
		__attribute__((always_inline)) constexpr inline vec2 &operator/=(float const scalar);

		__attribute__((always_inline)) constexpr inline bool operator==(vec2 const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(vec2 const other) const;

		static float dot_product(vec2 const first, vec2 const second);

		float len() const;
		float sqr_len() const;
		vec2 const normalized() const;
		vec2 const perpendicular() const;
		float angle() const;

		static float angle_between(vec2 const first, vec2 const second);
		static vec2 const rotate(vec2 const vec, float const angle);
		static vec2 const rotate_with_known_trig(vec2 const vec, float const cos, float const sin);
	};
}

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
