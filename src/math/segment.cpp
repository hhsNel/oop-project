#include "segment.h"

math::fxpt const math::segment::angle() const {
	return (point1 - point0).angle();
}

math::fxpt const math::segment::len() const {
	return (point1 - point0).len();
}

math::fxpt const math::segment::sqr_len() const {
	return (point1 - point0).sqr_len();
}
