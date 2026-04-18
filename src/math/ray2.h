#pragma once
#define RAY2_H

#include "fxpt.h"
#include "vec2.h"
#include "segment.h"

namespace math {
	class ray2 {
	public:
		vec2 end;
		vec2 direction;

		constexpr ray2(vec2 const e, vec2 const d);

		__attribute__((always_inline)) constexpr inline bool operator==(ray2 const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(ray2 const other) const;

		static ray2 const rotate(ray2 const ray, fxpt const angle);

		bool intersects(segment const seg, vec2 &hit_point, fxpt &distance, fxpt &seg_len) const;
	};
}

__attribute__((always_inline)) constexpr inline bool
math::ray2::operator==(ray2 const other) const {
	return end == other.end && direction == other.direction;
}

__attribute__((always_inline)) constexpr inline bool
math::ray2::operator!=(ray2 const other) const {
	return !(*this == other);
}