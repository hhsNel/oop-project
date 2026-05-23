#pragma once
#define SECTOR_H

#include "assets/asset-manager.h"
#include "util/resource.h"
#include <vector>
#include <cstdint>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class sector {
		float floor_height;
		float ceiling_height;
		assets::texture_id floor_tex;
		assets::texture_id ceiling_tex;
		std::uint8_t light_level;

	public:

		sector(float fh, float ch, assets::texture_id ft, assets::texture_id ct, std::uint8_t light);

		static std::vector<sector> load_from_bin(util::resource const& res);

		friend rendering::software_renderer;
	};
}
