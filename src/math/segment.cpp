#include "segment.h"

float math::segment::angle() const {
	return (point1 - point0).angle();
}

float math::segment::len() const {
	return (point1 - point0).len();
}

float math::segment::sqr_len() const {
	return (point1 - point0).sqr_len();
}
