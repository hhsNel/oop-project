#pragma once
#define RENDERER_2D_H

#include "rendering-backend.h"
#include "assets/asset-manager.h"
#include "assets/texture.h"
#include <string_view>
#include <cstdint>
#include <algorithm>

namespace rendering {
	class renderer_2d {
		rendering_backend &target;
		assets::asset_manager const& tex_manager;
		assets::texture const& font_texture; 

	public:
		renderer_2d(rendering_backend &tgt, assets::asset_manager const& tm, assets::texture const& font_tex);

		void draw_texture(assets::texture const& tex, int x, int y, int w, int h) const;
		
		void draw_text(std::string_view text, int x, int y, int char_w, int char_h, std::uint32_t const color) const;

		void draw_rect(int x, int y, int w, int h, std::uint32_t color) const;
	};
}
