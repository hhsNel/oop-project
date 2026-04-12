#pragma once
#define FXPT_H

#include <cstddef>
#include <cstdint>

namespace math {
	class fxpt {
		std::uint32_t raw;
	public:
		fxpt(float const f);
		__attribute__((always_inline)) inline float const to_float() const;

		__attribute__((always_inline)) inline fxpt const operator+(fxpt const other) const;
		__attribute__((always_inline)) inline fxpt const operator-(fxpt const other) const;
		__attribute__((always_inline)) inline fxpt const operator*(fxpt const other) const;
		__attribute__((always_inline)) inline fxpt const operator/(fxpt const other) const;
		__attribute__((always_inline)) inline fxpt const operator+=(fxpt const other);
		__attribute__((always_inline)) inline fxpt const operator-=(fxpt const other);
		__attribute__((always_inline)) inline fxpt const operator*=(fxpt const other);
		__attribute__((always_inline)) inline fxpt const operator/=(fxpt const other);

		fxpt const sqr() const;
		fxpt const sqrt() const;

		static fxpt const sin(fxpt const angle);
		static fxpt const cos(fxpt const angle);
		static fxpt const atan2(fxpt const x, fxpt const y);
	};
}

