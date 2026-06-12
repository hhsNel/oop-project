#pragma once

#include "vec2.h"
#include "util/componentized.h"

namespace math {
	class segment : public util::componentized<segment> {
		/* segment points */
		[[=util::component_field{}]] vec2 point0, point1;

	public:

		/* constructor */
		constexpr segment(vec2 const p0, vec2 const p1);

		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator==(segment const other) const;
		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator!=(segment const other) const;

		/* segment angle */
		float angle() const;
		/* segment midpoint */
		constexpr vec2 const midpoint() const;
		/* segment length */
		float len() const;
		/* segment square length */
		float sqr_len() const;

		friend util::componentized<segment>;
	};
}

/* constexpr inline definitions */
constexpr math::segment::segment(vec2 const p0, vec2 const p1) : point0(p0), point1(p1) {}

constexpr math::vec2 const math::segment::midpoint() const {
	return (point0 + point1) / 2.0f;
}

__attribute__((always_inline)) constexpr inline bool
math::segment::operator==(segment const other) const {
	return point0 == other.point0 && point1 == other.point1;
}

__attribute__((always_inline)) constexpr inline bool
math::segment::operator!=(segment const other) const {
	return !(*this == other);
}
