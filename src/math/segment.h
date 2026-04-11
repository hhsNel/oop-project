#pragma once
#define SEGMENT_H

#include "fxpt.h"
#include "vec2.h"

namespace math {
	class segment {
	public:
		vec2 point0, point1;

		segment(vec2 const p0, vec2 const p1);

		fxpt const angle() const;
		vec2 const midpoint() const;
		fxpt const len() const;
		fxpt const sqr_len() const;
	};
}

