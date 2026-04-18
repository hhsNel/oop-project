#include "segment.h"

constexpr math::segment::segment(vec2 const p0, vec2 const p1) : point0(p0), point1(p1) {}

math::fxpt const math::segment::angle() const {
	return (point1 - point0).angle();
}

constexpr math::vec2 const math::segment::midpoint() const {
	return (point0 + point1) / fxpt(2.0f);
}

math::fxpt const math::segment::len() const {
	return (point1 - point0).len();
}

math::fxpt const math::segment::sqr_len() const {
	return (point1 - point0).sqr_len();
}