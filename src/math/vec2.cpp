#include "vec2.h"
#include "vec3.h"

#include <cmath>

math::vec2::vec2(vec3 const& vec) : x(vec.x), y(vec.y) {}

float
math::vec2::dot_product(vec2 const first, vec2 const second) {
	return first.x * second.x + first.y * second.y;
}

float
math::vec2::sqr_len() const {
	return dot_product(*this, *this);
}

float
math::vec2::len() const {
	return std::sqrt(x*x+y*y);
}

math::vec2 const
math::vec2::normalized() const {
	float length = len();
	if (!length) {
		return vec2(float(0.0f), float(0.0f));
	}
	return *this / length;
}

math::vec2 const
math::vec2::perpendicular() const {
	return vec2(-y, x);
}

float
math::vec2::angle() const {
	return std::atan2(y, x);
}

float
math::vec2::angle_between(vec2 const first, vec2 const second) {
	float dot = dot_product(first, second);
	float len1 = first.len();
	float len2 = second.len();

	if (!len1 || !len2) {
		return float(0.0f);
	}

	float cos_theta = dot / (len1 * len2);

	if (cos_theta > float(1.0f))  cos_theta = float(1.0f);
	if (cos_theta < float(-1.0f)) cos_theta = float(-1.0f);

	return std::acos(cos_theta);
}

math::vec2 const
math::vec2::rotate(vec2 const vec, float const angle) {
	float c = std::cos(angle);
	float s = std::sin(angle);

	return vec2(
		vec.x * c - vec.y * s,
		vec.x * s + vec.y * c
	);
}

math::vec2 const
math::vec2::rotate_with_known_trig(vec2 const vec, float const cos, float const sin) {
	return vec2(
		vec.x * cos - vec.y * sin,
		vec.x * sin + vec.y * cos
	);
}

