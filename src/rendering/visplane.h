#pragma once


#include <cstdint>
#include <vector>

#include "assets/asset-manager.h"
#include "assets/ids.h"

namespace rendering {
	struct frame_rendering_data;

	class visplane {
		float height;
		assets::texture_id tex_id;
		std::uint8_t light_level;
		int min_x, max_x;
		std::vector<int> top, bottom;

	public:

		visplane(unsigned int const sw, float const h, assets::texture_id const tid, std::uint8_t const light);

		static void add_column(std::vector<visplane>& pool, int x, int y_start, int y_end, unsigned int sw, float flat_height, assets::texture_id tex_id, std::uint8_t light_level);

		void render(assets::asset_manager const& tex_manager, frame_rendering_data const& frd) const;
	};
}

