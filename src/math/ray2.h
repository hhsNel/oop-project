#pragma once

#include "vec2.h"
#include "segment.h"

namespace math {
	class ray2 {
	public:
		/* ray end */
		vec2 end;
		/* ray direction */
		vec2 direction;

		/* constructor */
		constexpr ray2(vec2 const e, vec2 const d);

		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator==(ray2 const other) const;
		/* comparison operators */
		__attribute__((always_inline)) constexpr inline bool
			operator!=(ray2 const other) const;

		/* rotate ray by angle */
		static ray2 const rotate(ray2 const ray, float const angle);

		/* ray intersects a segment */
		bool intersects(segment const seg, vec2 &hit_point, float &distance,
			float &seg_len) const;
	};
}

/* constexpr inline definitions */
constexpr math::ray2::ray2(vec2 const e, vec2 const d) : end(e), direction(d) {}

__attribute__((always_inline)) constexpr inline bool
math::ray2::operator==(ray2 const other) const {
	return end == other.end && direction == other.direction;
}

__attribute__((always_inline)) constexpr inline bool
math::ray2::operator!=(ray2 const other) const {
	return !(*this == other);
}

