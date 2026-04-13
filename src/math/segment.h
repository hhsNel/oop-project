#pragma once
#define SEGMENT_H

#include "fxpt.h"
#include "vec2.h"

namespace math {
	class segment {
	public:
		vec2 point0, point1;

		constexpr segment(vec2 const p0, vec2 const p1);

		constexpr fxpt const angle() const;
		constexpr vec2 const midpoint() const;
		constexpr fxpt const len() const;
		constexpr fxpt const sqr_len() const;
	};
}

