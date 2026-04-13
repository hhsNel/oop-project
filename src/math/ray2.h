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

		constexpr static ray2 const rotate(ray2 const ray, fxpt const angle);

		constexpr bool const intersects(segment const seg, vec2 &hit_point, fxpt &distance, fxpt &seg_len) const;
	};
}

