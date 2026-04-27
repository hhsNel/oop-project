#include "ray2.h"

math::ray2 const math::ray2::rotate(ray2 const ray, float const angle) {
	return ray2(ray.end, vec2::rotate(ray.direction, angle));
}

bool math::ray2::intersects(segment const seg, vec2 &hit_point, float &distance, float &seg_len) const {
	vec2 p = end;
	vec2 r = direction;
	vec2 q = seg.point0;
	vec2 s = seg.point1 - seg.point0;

	float r_cross_s = r.x * s.y - r.y * s.x;
	vec2 q_minus_p = q - p;
	float q_minus_p_cross_r = q_minus_p.x * r.y - q_minus_p.y * r.x;

	if (!r_cross_s) { 
		return false;
	}

	float t = (q_minus_p.x * s.y - q_minus_p.y * s.x) / r_cross_s;
	float u = q_minus_p_cross_r / r_cross_s;

	if (t >= 0.0f && u >= 0.0f && u <= 1.0f) {
		hit_point = p + r * t;
		distance = t * r.len();
		seg_len = u * s.len();
		return true;
	}

	return false;
}
