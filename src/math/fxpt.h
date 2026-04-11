#pragma once
#define FXPT_H

#include <cstddef>

namespace math {
	class fxpt {
		uint32_t raw;
	public:
		fxpt(float const f);
		float const to_float() const;

		fxpt const operator+(fxpt const other) const;
		fxpt const operator-(fxpt const other) const;
		fxpt const operator*(fxpt const other) const;
		fxpt const operator/(fxpt const other) const;
		fxpt const operator+=(fxpt const other);
		fxpt const operator-=(fxpt const other);
		fxpt const operator*=(fxpt const other);
		fxpt const operator/=(fxpt const other);

		fxpt const sqr() const;
		fxpt const sqrt() const;

		static fxpt const sin(fxpt const angle);
		static fxpt const cos(fxpt const angle);
		static fxpt const atan2(fxpt const x, fxpt const y);
	};
}

