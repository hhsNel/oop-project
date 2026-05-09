#pragma once
#define RENDERER_2D_H

#include "rendering-backend.h"
#include "graphics/texture-manager.h"
#include "graphics/texture.h"
#include <string_view>
#include <cstdint>
#include <algorithm>

namespace rendering {
    class renderer_2d {
        rendering_backend &target;
        graphics::texture_manager const& tex_manager;
        graphics::texture const& font_texture; 

    public:
        renderer_2d(rendering_backend &tgt, graphics::texture_manager const& tm, graphics::texture const& font_tex);

        void draw_texture(graphics::texture const& tex, int x, int y, int w, int h);
        
        void draw_text(std::string_view text, int x, int y, int char_w, int char_h, std::uint32_t const color);

		void draw_rect(int x, int y, int w, int h, std::uint32_t color);
    };
}
