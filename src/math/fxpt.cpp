#include "fxpt.h"

math::fxpt const math::fxpt::sin(math::fxpt const angle) {
	return math::fxpt(std::sin(angle.to_float()));
}

math::fxpt const math::fxpt::cos(math::fxpt const angle) {
	return math::fxpt(std::cos(angle.to_float()));
}

math::fxpt const math::fxpt::acos(math::fxpt const val) {
	return math::fxpt(std::acos(val.to_float()));
}

math::fxpt const math::fxpt::atan2(math::fxpt const y, math::fxpt const x) {
	return math::fxpt(std::atan2(y.to_float(), x.to_float()));
}