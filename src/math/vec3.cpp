#include "vec3.h"

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
