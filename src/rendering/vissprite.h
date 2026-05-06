#pragma once
#define VISSPRITE_H

#include <cstdint>
#include <vector>

#include "sprite.h"

namespace rendering {
	class vissprite {
	public:
		float depth;
		int cx1, cx2;
		float proj_x, scale;
		float z_pos;
		graphics::texture_set::texture_id tex_id;
		std::uint8_t light_level;

		std::vector<int> upper_clip;
		std::vector<int> lower_clip;

		vissprite(sprite const& sprite, float const z, int const clamped_x1, int const clamped_x2, float const px, float const sc, std::uint8_t const light, std::vector<int> const& uc, std::vector<int> const& lc);
	};
}

