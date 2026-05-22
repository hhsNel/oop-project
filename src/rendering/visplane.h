#pragma once
#define VISPLANE_H

#include <cstdint>
#include <vector>

#include "assets/ids.h"

namespace rendering {
	class visplane {
		/* TODO */
	public:
		float height;
		assets::texture_id tex_id;
		std::uint8_t light_level;

		int min_x, max_x;
		std::vector<int> top, bottom;

		visplane(int const x, unsigned int const sw, float const h, assets::texture_id const tid, std::uint8_t const light);
	};
}

