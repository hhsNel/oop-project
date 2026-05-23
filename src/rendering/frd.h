#pragma once

#include <cstdint>
#include <vector>
#include "math/vec2.h"

namespace rendering {
	// All per-frame, camera-derived constants that rendering objects need.
	// Computed once in software_renderer::render_bsp() and passed down by value.
	struct frame_rendering_data {
		math::vec2 cam_pos;
		float cam_height, cam_angle;
		unsigned int sw, sh;
		float half_sw;
		unsigned int pitch;
		std::uint32_t *__restrict mmio;
		float fov_scale;
		float inv_fov_scale;
		float cos_cam_angle, sin_cam_angle;

		// Per-column Euclidean distance correction factor (sqrt(dx^2 + 1)).
		// Stored here so visplanes and vissprites can access it without going
		// back through software_renderer.
		std::vector<float> const* euclidian_dist_factor;
	};
}