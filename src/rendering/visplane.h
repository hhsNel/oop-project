#pragma once
#define VISPLANE_H

#include <cstdint>
#include <vector>

#include "assets/texture-manager.h"

namespace rendering {
	class visplane {
		/* TODO */
	public:
		float height;
		assets::asset_pack::texture_id tex_id;
		std::uint8_t light_level;

		int min_x, max_x;
		std::vector<int> top, bottom;

		visplane(int const x, unsigned int const sw, float const h, assets::asset_pack::texture_id const tid, std::uint8_t const light);
	};
}

