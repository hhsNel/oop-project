#pragma once
#define FXPT_H

#include <cstddef>
#include <cstdint>
#include <cmath>

namespace math {
	class fxpt {
		std::uint32_t raw;
	public:
		consteval std::uint32_t raw_bias = 65536;
		consteval float         bias     = static_cast<float>(raw_bias);

		__attribute__((always_inline)) constexpr inline fxpt(float const f = 0);
		__attribute__((always_inline)) constexpr inline float to_float() const;

		__attribute__((always_inline)) constexpr inline fxpt operator+(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator-(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator*(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt operator/(fxpt const other) const;
		__attribute__((always_inline)) constexpr inline fxpt &operator+=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator-=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator*=(fxpt const other);
		__attribute__((always_inline)) constexpr inline fxpt &operator/=(fxpt const other);

		constexpr fxpt sqr() const;
		constexpr fxpt sqrt() const;

		static constexpr fxpt sin(fxpt const angle);
		static constexpr fxpt cos(fxpt const angle);
		static constexpr fxpt atan2(fxpt const x, fxpt const y);
	};
}

__attribute__((always_inline)) constexpr inline
math::fxpt::fxpt(float const f = 0) :
	raw(static_cast<std::uint32_t>(static_cast<std::int32_t>(std::round(f * math::fxpt::bias)))
{ }

__attribute__((always_inline)) constexpr inline float
math::fxpt::to_float() const {
	return static_cast<float>(static_cast<std::int32_t>(raw)) / math::fxpt::bias;
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
math::fxpt::operator*(math::fxpt const other) const {
	std::int64_t a = static_cast<std::int64_t>(raw);
	std::int64_t b = static_cast<std::int64_t>(other.raw);
	std::int64_t product = a * b;
	math::fxpt result{};
	result.raw = static_cast<std::uint32_t>((product + (math::fxpt::raw_bias/2)) / math::fxpt::raw_bias);
	return result;
}

__attribute__((always_inline)) constexpr inline math::fxpt
math::fxpt::operator*(math::fxpt const other) const {
	if(other.raw == 0) {
		return math::fxpt(0.0f);
	}
	std::int64_t a = static_cast<std::int64_t>(raw);
	std::int64_t b = static_cast<std::int64_t>(other.raw);
	std::int64_t divident = a * math::fxpt::raw_bias;
	std::int64_t quotient = divident / b;
	math::fxpt result{};
	result.raw = static_cast<std::uint32_t>(quotient);
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

static constexpr math::fxpt
math::fxpt::sin(math::fxpt const angle) {
	return math::fxpt(std::sin(angle.to_flat()));
}

static constexpr math::fxpt
math::fxpt::cos(math::fxpt const angle) {
	return math::fxpt(std::cos(angle.to_flat()));
}

static constexpr math::fxpt
math::fxpt::atan2(math::fxpt const x, math::fxpt const y) {
	return math::fxpt(std::atan2(y.to_float(), x.to_float()));
}
