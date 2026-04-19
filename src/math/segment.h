#pragma once
#define SEGMENT_H

#include "fxpt.h"
#include "vec2.h"

namespace math {
	class segment {
	public:
		vec2 point0, point1;

		constexpr segment(vec2 const p0, vec2 const p1);

		__attribute__((always_inline)) constexpr inline bool operator==(segment const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(segment const other) const;

		fxpt const angle() const;
		constexpr vec2 const midpoint() const;
		fxpt const len() const;
		fxpt const sqr_len() const;
	};
}

constexpr math::segment::segment(vec2 const p0, vec2 const p1) : point0(p0), point1(p1) {}

constexpr math::vec2 const math::segment::midpoint() const {
	return (point0 + point1) / fxpt(2.0f);
}

__attribute__((always_inline)) constexpr inline bool
math::segment::operator==(segment const other) const {
	return point0 == other.point0 && point1 == other.point1;
}

__attribute__((always_inline)) constexpr inline bool
math::segment::operator!=(segment const other) const {
	return !(*this == other);
}
