#include "vec2.h"
#include "vec3.h"

math::vec2::vec2(vec3 const& vec) : a(vec.a), b(vec.b) {}

math::fxpt const
math::vec2::dot_product(vec2 const first, vec2 const second) {
	return first.a * second.a + first.b * second.b;
}

math::fxpt const
math::vec2::sqr_len() const {
	return dot_product(*this, *this);
}

math::fxpt const
math::vec2::len() const {
	return sqr_len().sqrt();
}

math::vec2 const
math::vec2::normalized() const {
	fxpt length = len();
	if (!length) {
		return vec2(fxpt(0.0f), fxpt(0.0f));
	}
	return *this / length;
}

math::vec2 const
math::vec2::perpendicular() const {
	return vec2(-b, a);
}

math::fxpt const
math::vec2::angle() const {
	return math::fxpt::atan2(b, a);
}

math::fxpt const
math::vec2::angle_between(vec2 const first, vec2 const second) {
	fxpt dot = dot_product(first, second);
	fxpt len1 = first.len();
	fxpt len2 = second.len();

	if (!len1 || !len2) {
		return fxpt(0.0f);
	}

	fxpt cos_theta = dot / (len1 * len2);

	if (cos_theta > fxpt(1.0f))  cos_theta = fxpt(1.0f);
	if (cos_theta < fxpt(-1.0f)) cos_theta = fxpt(-1.0f);

	return fxpt::acos(cos_theta);
}

math::vec2 const
math::vec2::rotate(vec2 const vec, fxpt const angle) {
	fxpt c = math::fxpt::cos(angle);
	fxpt s = math::fxpt::sin(angle);

	return vec2(
		vec.a * c - vec.b * s,
		vec.a * s + vec.b * c
	);
}
