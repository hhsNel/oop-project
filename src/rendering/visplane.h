#pragma once


#include <cstdint>
#include <vector>

#include "assets/asset-manager.h"
#include "assets/ids.h"

namespace rendering {
	struct frame_rendering_data;

	class visplane {
		/* visplane height */
		float height;
		/* visplane texture */
		assets::texture_id tex_id;
		/* sector light */
		std::uint8_t light_level;
		/* visplane x span */
		int min_x, max_x;
		/* visplane bounding functions */
		std::vector<int> top, bottom;

	public:

		/* constructor */
		visplane(unsigned int const sw, float const h,
			assets::texture_id const tid, std::uint8_t const light);

		/* add column to a visplane vector, either by extending an existing visplane */
		/* or by creating a new one */
		static void add_column(std::vector<visplane>& pool, int x,
			int y_start, int y_end, unsigned int sw, float flat_height,
			assets::texture_id tex_id, std::uint8_t light_level);

		/* render a visplane */
		void render(assets::asset_manager const& tex_manager,
			frame_rendering_data const& frd) const;
	};
}

