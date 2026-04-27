#include "renderer-2d.h"

namespace rendering {

    renderer_2d::renderer_2d() : target(nullptr), tex_manager(nullptr), font_texture(nullptr) {}

    void renderer_2d::set_target(rendering_backend *const tgt) {
        target = tgt;
    }

    void renderer_2d::set_texture_manager(graphics::texture_manager const *const tm) {
        tex_manager = tm;
    }

    void renderer_2d::set_font_texture(graphics::texture const *const font_tex) {
        font_texture = font_tex;
    }

    void renderer_2d::draw_texture(graphics::texture const& tex, int x, int y, int w, int h) {
        if (!target || w <= 0 || h <= 0) return;

        int const screen_w = static_cast<int>(target->get_width());
        int const screen_h = static_cast<int>(target->get_height());
        
        unsigned int const pitch = target->get_pitch() / sizeof(std::uint32_t); 
        std::uint32_t *__restrict mmio = target->get_mmio();

        int const start_x = std::max(0, x);
        int const start_y = std::max(0, y);
        int const end_x   = std::min(screen_w, x + w);
        int const end_y   = std::min(screen_h, y + h);

        std::uint32_t const MAGENTA_COLORKEY = 0xFFFF00FF; 

		for (int dx = start_x; dx < end_x; ++dx) {
			std::uint32_t const src_x = ((dx - x) * tex.width) / w;
			for (int dy = start_y; dy < end_y; ++dy) {
				std::uint32_t const src_y = ((dy - y) * tex.height) / h;

				std::uint32_t const pixel = tex.pixels[src_x * tex.height + src_y];

                if (tex.has_transparency && pixel == MAGENTA_COLORKEY) {
                    continue; 
                }

                mmio[dy * pitch + dx] = pixel;
            }
        }
    }

    void renderer_2d::draw_text(std::string_view text, int x, int y, int char_w, int char_h) {
        if (!font_texture) return;

        int const cols = 16;
        int const rows = 16;
        int const src_char_w = font_texture->width / cols;
        int const src_char_h = font_texture->height / rows;

        int current_x = x;
        std::uint32_t const MAGENTA_COLORKEY = 0xFFFF00FF;

        for (char const c : text) {
            if (c == '\n') {
                current_x = x;
                y += char_h;
                continue;
            }

            unsigned char const char_idx = static_cast<unsigned char>(c);
            int const grid_x = char_idx % cols;
            int const grid_y = char_idx / rows;
            
            int const start_x = std::max(0, current_x);
            int const start_y = std::max(0, y);
            int const end_x   = std::min(static_cast<int>(target->get_width()), current_x + char_w);
            int const end_y   = std::min(static_cast<int>(target->get_height()), y + char_h);

            unsigned int const pitch = target->get_pitch() / sizeof(std::uint32_t);
            std::uint32_t *__restrict mmio = target->get_mmio();

			for (int dx = start_x; dx < end_x; ++dx) {
				std::uint32_t const src_x = grid_x * src_char_w + ((dx - current_x) * src_char_w) / char_w;
				for (int dy = start_y; dy < end_y; ++dy) {
					std::uint32_t const src_y = grid_y * src_char_h + ((dy - y) * src_char_h) / char_h;

					std::uint32_t const pixel = font_texture->pixels[src_x * font_texture->height + src_y];

                    if (font_texture->has_transparency && pixel == MAGENTA_COLORKEY) {
                        continue;
                    }

                    mmio[dy * pitch + dx] = pixel;
                }
            }

            current_x += char_w;
        }
    }
}
