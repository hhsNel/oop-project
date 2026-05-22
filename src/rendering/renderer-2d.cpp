#include "renderer-2d.h"

namespace rendering {

    renderer_2d::renderer_2d(rendering_backend &tgt, assets::asset_manager const& tm, assets::texture const& font_tex) : target(tgt), tex_manager(tm), font_texture(font_tex) {}

    void renderer_2d::draw_texture(assets::texture const& tex, int x, int y, int w, int h) const {
        int const screen_w = static_cast<int>(target("width"_f));
        int const screen_h = static_cast<int>(target("height"_f));
        
        unsigned int const pitch = target("pitch"_f) / sizeof(std::uint32_t); 
        std::uint32_t *__restrict mmio = target("mmio"_f);

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

    void renderer_2d::draw_text(std::string_view text, int x, int y, int char_w, int char_h, std::uint32_t const color) const {
        int const cols = 16;
        int const rows = 16;
        int const src_char_w = font_texture.width / cols;
        int const src_char_h = font_texture.height / rows;

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
            int const end_x   = std::min(static_cast<int>(target("width"_f)), current_x + char_w);
            int const end_y   = std::min(static_cast<int>(target("height"_f)), y + char_h);

            unsigned int const pitch = target("pitch"_f) / sizeof(std::uint32_t);
            std::uint32_t *__restrict mmio = target("mmio"_f);

			for (int dx = start_x; dx < end_x; ++dx) {
				std::uint32_t const src_x = grid_x * src_char_w + ((dx - current_x) * src_char_w) / char_w;
				for (int dy = start_y; dy < end_y; ++dy) {
					std::uint32_t const src_y = grid_y * src_char_h + ((dy - y) * src_char_h) / char_h;

					std::uint32_t const pixel = font_texture.pixels[src_x * font_texture.height + src_y];

                    if (font_texture.has_transparency && pixel == MAGENTA_COLORKEY) {
                        continue;
                    }

                    mmio[dy * pitch + dx] = pixel * color;
                }
            }

            current_x += char_w;
        }
    }

    void renderer_2d::draw_rect(int x, int y, int w, int h, std::uint32_t color) const {
		auto* fb    = target("mmio"_f);
		unsigned pitch = target("pitch"_f) / sizeof(std::uint32_t);
		unsigned scr_w = target("width"_f);
		unsigned scr_h = target("height"_f);
		int x1 = std::max(0, x),              y1 = std::max(0, y);
		int x2 = std::min((int)scr_w, x + w), y2 = std::min((int)scr_h, y + h);

		std::uint32_t alpha = (color >> 24) & 0xFF;
		if (alpha == 0xFF) {
			for (int row = y1; row < y2; ++row)
			for (int col = x1; col < x2; ++col)
				fb[row * pitch + col] = color;
		} else {
			std::uint32_t inv = 255 - alpha;
			std::uint32_t sr = (color >> 16) & 0xFF;
			std::uint32_t sg = (color >>  8) & 0xFF;
			std::uint32_t sb = (color      ) & 0xFF;
			for (int row = y1; row < y2; ++row) {
			for (int col = x1; col < x2; ++col) {
				std::uint32_t dst = fb[row * pitch + col];
				std::uint32_t r = (sr * alpha + ((dst >> 16) & 0xFF) * inv) / 255;
				std::uint32_t g = (sg * alpha + ((dst >>  8) & 0xFF) * inv) / 255;
				std::uint32_t b = (sb * alpha + ((dst      ) & 0xFF) * inv) / 255;
				fb[row * pitch + col] = (0xFF << 24) | (r << 16) | (g << 8) | b;
			}
			}
		}
    }
}
