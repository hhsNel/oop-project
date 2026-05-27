#include "vissprite.h"

#include <algorithm>

#include "lighting.h"
#include "frd.h"

rendering::vissprite::vissprite(sprite const& sprite, float const z, int const clamped_x1, int const clamped_x2, float const px, float const sc, std::uint8_t const light, std::vector<int> const& uc, std::vector<int> const& lc) :
	depth(z),
	cx1(clamped_x1),
	cx2(clamped_x2),
	proj_x(px),
	scale(sc),
	z_pos(sprite.z_pos),
	tex_id(sprite.tex_id),
	light_level(light),
	flash_red(sprite.hit_flash > 0.0f),
	upper_clip(uc),
	lower_clip(lc) { }

void rendering::vissprite::pa_sort(std::vector<vissprite> &vec) {
	std::sort(vec.begin(), vec.end(), [](vissprite const& a, vissprite const& b) {
			return a.depth > b.depth;
		});
}

void rendering::vissprite::render(assets::asset_manager const& tex_manager, frame_rendering_data const& frd) const {
	assets::texture const& tex = tex_manager.sprite_tx_by_id(tex_id);
	float eu_dist_f = (*frd.euclidian_dist_factor)[std::clamp(static_cast<int>(proj_x), 0, static_cast<int>(frd.sw) - 1)];
	int sprite_light = lighting::calculate(light_level, depth * eu_dist_f);

	float scr_y_bot = frd.sh/2.0f - (z_pos - frd.cam_height) * scale;
	float scr_y_top = scr_y_bot - tex("height"_f) * scale;

	float inv_scale = 1.0f / scale;
	
	for(int x = cx1; x <= cx2; ++x) {
		int clip_top = upper_clip[x - cx1];
		int clip_bot = lower_clip[x - cx1];

		/* occlusion culling */
		if(clip_bot <= clip_top) continue;

		float draw_top = std::max(static_cast<float>(std::max(0, clip_top)), scr_y_top);
		float draw_bot = std::min(static_cast<float>(clip_bot), scr_y_bot);

		if(draw_bot <= draw_top) continue;

		/* should be correct ??? */
		int u = static_cast<int>((x - proj_x + (tex("width"_f)*scale)/2) * inv_scale);
		u = std::clamp(u, 0, static_cast<int>(tex("width"_f)) - 1);

		float current_v = (draw_top - scr_y_top) * inv_scale;

		for(int y = static_cast<int>(draw_top); y < static_cast<int>(draw_bot); ++y) {
			int v = static_cast<int>(current_v);
			current_v += inv_scale;

			std::uint32_t color = tex("pixels"_f)[u * tex("height"_f) + v];
			if(color != 0xffff00ff) {
				std::uint32_t lit = lighting::apply(color, sprite_light);
				if (flash_red) {
					std::uint32_t r = std::min(255u, ((lit >> 16) & 0xFF) + 180u);
					std::uint32_t g = ((lit >> 8) & 0xFF) / 3;
					std::uint32_t b = (lit & 0xFF) / 3;
					lit = (lit & 0xFF000000) | (r << 16) | (g << 8) | b;
				}
				frd.mmio[x + y * frd.pitch] = lit;
			}
		}
	}
}
