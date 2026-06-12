#pragma once


#include "rendering-backend.h"
#include "assets/asset-manager.h"
#include "assets/texture.h"
#include <string_view>
#include <cstdint>
#include <algorithm>

namespace rendering {
	class renderer_2d {
		/* chosen backend */
		rendering_backend &target;
		/* asset mgr ref */
		assets::asset_manager const& tex_manager;
		/* chosen 16x16 font atlas */
		assets::texture const& font_texture; 

	public:
		/* constructor */
		renderer_2d(rendering_backend &tgt, assets::asset_manager const& tm,
			assets::texture const& font_tex);

		/* blit texture onto backend */
		void draw_texture(assets::texture const& tex, int x, int y, int w, int h) const;
		
		/* draw parts of font atlas onto backend, forming a string */
		void draw_text(std::string_view text, int x, int y, int char_w,
			int char_h, std::uint32_t const color) const;

		/* draw rectangle with alpha */
		void draw_rect(int x, int y, int w, int h, std::uint32_t color) const;
	};
}
