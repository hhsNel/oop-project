#pragma once

#include <cstdint>
#include <vector>

#include "assets/asset-manager.h"
#include "sprite.h"

namespace rendering {
	struct frame_rendering_data;

	class vissprite {
		/* z */
		float depth;
		/* clamped screen position */
		int cx1, cx2;
		/* projected position, sprite scale */
		float proj_x, scale;
		/* projected height */
		float z_pos;
		/* sprite texture id */
		assets::texture_id tex_id;
		/* sector light level */
		std::uint8_t light_level;
		/* damage flash */
		bool flash_red;

		/* upper clip array at vissprite queue time */
		std::vector<int> upper_clip;
		/* lower clip array at vissprite queue time */
		std::vector<int> lower_clip;

	public:

		/* constructor */
		vissprite(sprite const& sprite, float const z, int const clamped_x1,
			int const clamped_x2, float const px, float const sc,
			std::uint8_t const light, std::vector<int> const& uc,
			std::vector<int> const& lc);

		/* sort for painters algorithm */
		static void pa_sort(std::vector<vissprite> &vec);

		/* render onto backend */
		void render(assets::asset_manager const& tex_manager,
			frame_rendering_data const& frd) const;
	};
}
