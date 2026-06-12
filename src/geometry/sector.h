#pragma once

#include "assets/asset-manager.h"
#include "util/resource.h"
#include <vector>
#include <cstdint>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class sector {
		/* sector floor */
		float floor_height;
		/* ceiling floor */
		float ceiling_height;
		/* sector floor texture */
		assets::texture_id floor_tex;
		/* ceiling floor texture */
		assets::texture_id ceiling_tex;
		/* sector lighting */
		std::uint8_t light_level;

	public:

		/* constructor */
		sector(float fh, float ch, assets::texture_id ft, assets::texture_id ct, std::uint8_t light);

		/* load sectors from a binary */
		static std::vector<sector> load_from_bin(util::resource const& res);

		friend rendering::software_renderer;
	};
}
