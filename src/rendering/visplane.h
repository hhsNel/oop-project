#pragma once
#define VISPLANE_H

#include <cstdint>
#include <vector>

#include "graphics/texture-manager.h"

namespace rendering {
	class visplane {
	public:
		float height;
		graphics::texture_manager::texture_id tex_id;
		std::uint8_t light_level;

		int min_x, max_x;
		std::vector<int> top, bottom;

		visplane(int const x, unsigned int const sw, float const h, graphics::texture_manager::texture_id const tid, std::uint8_t const light);
	};
}

