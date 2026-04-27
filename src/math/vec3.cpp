#include "vec3.h"

#include <cmath>

constexpr float math::vec3::sqr_len() const {
	return dot_product(*this, *this);
}

constexpr float math::vec3::len() const {
	return std::sqrt(sqr_len());
}

constexpr math::vec3 const math::vec3::normalized() const {
	float length = len();
	if (!length) {
		return vec3(0.0f, 0.0f, 0.0f);
	}
	return *this / length;
}
