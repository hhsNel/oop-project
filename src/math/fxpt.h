#pragma once
#define FXPT_H

#include <cstddef>
#include <cstdint>
#include <cmath>

namespace math {
	class fxpt {
		std::int32_t raw;
	public:
		static constexpr std::int32_t raw_bias = 65536;
		static constexpr float         bias     = static_cast<float>(raw_bias);

		__attribute__((always_inline)) constexpr inline fxpt(float const f = 0);
		__attribute__((always_inline)) constexpr inline float to_float() const;

		__attribute__((always_inline)) explicit constexpr inline operator bool() const;
		__attribute__((always_inline)) explicit constexpr inline operator float() const;
		__attribute__((always_inline)) explicit constexpr inline operator double() const;
		__attribute__((always_inline)) explicit constexpr inline operator int() const;

		__attribute__((always_inline)) constexpr inline fxpt operator+(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator-(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator-() const;
		__attribute__((always_inline)) constexpr inline fxpt operator*(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator/(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt &operator+=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator-=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator*=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator/=(fxpt const other);

		__attribute__((always_inline)) constexpr inline bool operator==(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline bool operator!=(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline bool operator<(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline bool operator>(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline bool operator<=(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline bool operator>=(fxpt const other) const;

		constexpr fxpt sqr() const;
		constexpr fxpt sqrt() const;
		constexpr bool is_zero() const;

		static fxpt const sin(fxpt const angle);
		static fxpt const cos(fxpt const angle);
		static fxpt const acos(fxpt const val);
		static fxpt const atan2(fxpt const y, fxpt const x);
	};
}

__attribute__((always_inline)) constexpr inline
math::fxpt::fxpt(float const f) :
	raw(static_cast<std::int32_t>(std::round(f * math::fxpt::bias)))
{ }

__attribute__((always_inline)) constexpr inline float
math::fxpt::to_float() const {
	return static_cast<float>(raw) / math::fxpt::bias;
}

__attribute__((always_inline)) constexpr inline
math::fxpt::operator bool() const {
	return raw != 0;
}

__attribute__((always_inline)) constexpr inline
math::fxpt::operator float() const {
	return to_float();
}

__attribute__((always_inline)) constexpr inline
math::fxpt::operator double() const {
	return static_cast<double>(raw) / math::fxpt::bias;
}

__attribute__((always_inline)) constexpr inline
math::fxpt::operator int() const {
	return static_cast<int>(raw / math::fxpt::raw_bias);
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator+(math::fxpt const other) const {
	math::fxpt result{};
	result.raw = raw + other.raw;
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator-(math::fxpt const other) const {
	math::fxpt result{};
	result.raw = raw - other.raw;
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator-() const {
	math::fxpt result{};
	result.raw = -raw;
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator*(math::fxpt const other) const {
	std::int64_t a = static_cast<std::int64_t>(raw);
	std::int64_t b = static_cast<std::int64_t>(other.raw);
	std::int64_t product = a * b;
	math::fxpt result{};
	result.raw = (product + (math::fxpt::raw_bias/2)) / math::fxpt::raw_bias;
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator/(math::fxpt const other) const {
	if(other.raw == 0) {
		return math::fxpt(0.0f);
	}
	std::int64_t a = static_cast<std::int64_t>(raw);
	std::int64_t b = static_cast<std::int64_t>(other.raw);
	std::int64_t divident = a * math::fxpt::raw_bias;
	std::int64_t quotient = divident / b;
	math::fxpt result{};
	result.raw = quotient;
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt &
math::fxpt::operator+=(math::fxpt const other) {
	raw += other.raw;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::fxpt &
math::fxpt::operator-=(math::fxpt const other) {
	raw -= other.raw;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::fxpt &
math::fxpt::operator*=(math::fxpt const other) {
	*this = *this * other;
	return *this;
}

__attribute__((always_inline)) constexpr inline math::fxpt &
math::fxpt::operator/=(math::fxpt const other) {
	*this = *this / other;
	return *this;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator==(math::fxpt const other) const {
	return raw == other.raw;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator!=(math::fxpt const other) const {
	return raw != other.raw;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator<(math::fxpt const other) const {
	return raw < other.raw;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator>(math::fxpt const other) const {
	return raw > other.raw;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator<=(math::fxpt const other) const {
	return raw <= other.raw;
}

__attribute__((always_inline)) constexpr inline bool
math::fxpt::operator>=(math::fxpt const other) const {
	return raw >= other.raw;
}

constexpr math::fxpt
math::fxpt::sqr() const {
	return *this * *this;
}

constexpr math::fxpt
math::fxpt::sqrt() const {
	float fval = to_float();
	if(fval < 0) return math::fxpt(0.0f);
	return math::fxpt(std::sqrt(fval));
}

constexpr bool
math::fxpt::is_zero() const {
	return raw == 0;
}
