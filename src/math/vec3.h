#pragma once
#define VEC3_H

#include "fxpt.h"
#include "vec2.h"

namespace math {
	class vec3 {
	public:
		fxpt a, b, c;

		vec3(fxpt const A = 0, fxpt const B = 0, fxpt const C = 0);
		vec3(vec2 const flat, fxpt const z = 0);

		vec3 const operator+(vec3 const other) const;
		vec3 const operator-(vec3 const other) const;
		vec3 const operator*(fxpt const scalar) const;
		vec3 const operator/(fxpt const scalar) const;
		vec3 const operator+=(vec3 const other);
		vec3 const operator-=(vec3 const other);
		vec3 const operator*=(fxpt const scalar);
		vec3 const operator/=(fxpt const scalar);

		static fxpt const dot_product(vec3 const first, vec3 const second);
		static vec3 const cross_product(vec3 const first, vec3 const second);

		fxpt const len() const;
		fxpt const sqr_len() const;
		vec3 const normalized() const;
	};
}

