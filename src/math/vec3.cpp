#include "vec3.h"

constexpr math::fxpt const math::vec3::dot_product(vec3 const first, vec3 const second) {
	return first.a * second.a + first.b * second.b + first.c * second.c;
}

constexpr math::vec3 const math::vec3::cross_product(vec3 const first, vec3 const second) {
	return vec3(
		first.b * second.c - first.c * second.b,
		first.c * second.a - first.a * second.c,
		first.a * second.b - first.b * second.a
	);
}

constexpr math::fxpt const math::vec3::sqr_len() const {
	return dot_product(*this, *this);
}

constexpr math::fxpt const math::vec3::len() const {
	return sqr_len().sqrt();
}

constexpr math::vec3 const math::vec3::normalized() const {
	fxpt length = len();
	if (!length) {
		return vec3(0.0f, 0.0f, 0.0f);
	}
	return *this / length;
}