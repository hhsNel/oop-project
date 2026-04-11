#pragma once
#define VEC2_H

#include "fxpt.h"

namespace math {
	class vec3;

	class vec2 {
	public:
		fxpt a, b;

		vec2(fxpt const A = 0, fxpt const B = 0);
		vec2(vec3 const vec);

		vec2 const operator+(vec2 const other) const;
		vec2 const operator-(vec2 const other) const;
		vec2 const operator*(fxpt const scalar) const;
		vec2 const operator/(fxpt const scalar) const;
		vec2 const operator+=(vec2 const other);
		vec2 const operator-=(vec2 const other);
		vec2 const operator*=(fxpt const scalar);
		vec2 const operator/=(fxpt const scalar);

		static fxpt const dot_product(vec2 const first, vec2 const second);

		fxpt const len() const;
		fxpt const sqr_len() const;
		vec2 const normalized() const;
		vec2 const perpendicular() const;
		fxpt const angle() const;

		static fxpt const angle_between(vec2 const first, vec2 const second);
		static vec2 const rotate(vec2 const vec, fxpt const angle);
	};
}

