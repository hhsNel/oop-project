#pragma once

#include <cstdint>
#include <vector>
#include "math/vec2.h"

namespace rendering {
	struct frame_rendering_data {
		/* camera position */
		math::vec2 cam_pos;
		/* camera height and angle */
		float cam_height, cam_angle;
		/* screen width and height */
		unsigned int sw, sh;
		/* half of screen width */
		float half_sw;
		/* backend pitch */
		unsigned int pitch;
		/* backend framebuffer */
		std::uint32_t *__restrict mmio;
		/* fov scale */
		float fov_scale;
		/* 1 / fov_scale */
		float inv_fov_scale;
		/* cosine and sine of camera angle */
		float cos_cam_angle, sin_cam_angle;

		/* euclidian distance correction factor */
		std::vector<float> const* euclidian_dist_factor;
	};
}
