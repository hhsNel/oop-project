#include "ray2.h"

math::ray2 const math::ray2::rotate(ray2 const ray, fxpt const angle) {
	return ray2(ray.end, vec2::rotate(ray.direction, angle));
}

bool math::ray2::intersects(segment const seg, vec2 &hit_point, fxpt &distance, fxpt &seg_len) const {
	vec2 p = end;
	vec2 r = direction;
	vec2 q = seg.point0;
	vec2 s = seg.point1 - seg.point0;

	fxpt r_cross_s = r.a * s.b - r.b * s.a;
	vec2 q_minus_p = q - p;
	fxpt q_minus_p_cross_r = q_minus_p.a * r.b - q_minus_p.b * r.a;

	if (!r_cross_s) { 
		return false;
	}

	fxpt t = (q_minus_p.a * s.b - q_minus_p.b * s.a) / r_cross_s;
	fxpt u = q_minus_p_cross_r / r_cross_s;

	if (t >= fxpt(0.0f) && u >= fxpt(0.0f) && u <= fxpt(1.0f)) {
		hit_point = p + r * t;
		distance = t * r.len();
		seg_len = u * s.len();
		return true;
	}

	return false;
}
