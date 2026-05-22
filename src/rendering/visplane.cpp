#include "visplane.h"

#include <cmath>
#include <algorithm>

#include "lighting.h"
#include "frd.h"

rendering::visplane::visplane(int const x, unsigned int const sw, float const h, assets::texture_id const tid, std::uint8_t const light) :
	height(h),
	tex_id(tid),
	light_level(light),
	min_x(x),
	max_x(x) {
	top.assign(sw, -1);
	bottom.assign(sw, -1);
}

void rendering::visplane::render(assets::asset_manager const& tex_manager, frame_rendering_data const& frd) const {
	/* optimizations */
    float dx_cos = frd.inv_fov_scale * frd.cos_cam_angle;
    float dx_sin = frd.inv_fov_scale * frd.sin_cam_angle;

    assets::texture const& tex = tex_manager.flat_tx_by_id(tex_id);
    float h = std::abs(frd.cam_height - height);

	/* true y bounds */
    int min_y = frd.sh, max_y = 0;
    for (int x = min_x; x <= max_x; ++x) {
        if (top[x] != -1) {
            min_y = std::min(min_y, (int)top[x]);
            max_y = std::max(max_y, (int)bottom[x]);
        }
    }

	/* draw loop; line by line */
    for (int y = min_y; y < max_y; ++y) {
        int dy = std::abs(y - (int)(frd.sh / 2));
        if (dy == 0) continue;

		/* constant depth z */
        float z = (h * frd.fov_scale) / (float)dy;
		/* because of that, constant light */
		/* also, no fisheye effect correction on flat lighting bc it would */
		/* be to expensive to calc lighting per-pixel */
        int row_light = lighting::calculate(light_level, z);

		/* steps in world space as fxpt 16.16*/
        int32_t step_x = (int32_t)(z * dx_cos * 65536.0f);
        int32_t step_y = (int32_t)(z * dx_sin * 65536.0f);

		/* scan row to find spans */
        int x = min_x;
        while (x <= max_x) {
			/* record span */
            while (x <= max_x && (top[x] == -1 || y < top[x] || y >= bottom[x])) {
                x++;
            }
            int span_start = x;

			/* find end of span */
            while (x <= max_x && (top[x] != -1 && y >= top[x] && y < bottom[x])) {
                x++;
            }
            int span_end = x;

			/* span ended, draw */

            float px = (float)span_start - frd.half_sw;
			/* grab world space coords */
            float world_x_f = z * (px * frd.inv_fov_scale * frd.cos_cam_angle - frd.sin_cam_angle) + frd.cam_pos("x"_f);
            float world_y_f = z * (px * frd.inv_fov_scale * frd.sin_cam_angle + frd.cos_cam_angle) + frd.cam_pos("y"_f);

			/* once again world coords as fxpt 16.16 to avoid fp math */
            int32_t fx = (int32_t)(world_x_f * 65536.0f);
            int32_t fy = (int32_t)(world_y_f * 65536.0f);

			/* current line ptr */
            uint32_t* dest = &frd.mmio[span_start + y * frd.pitch];

            for (int sx = span_start; sx < span_end; ++sx) {
				/* texture coords */
                int tx = ((fx >> 16) % tex.width);
                int ty = ((fy >> 16) % tex.height);
				/* ensure they are positive (modulo can be negative) */
                if (tx < 0) tx += tex.width;
                if (ty < 0) ty += tex.height;

				/* write the pixel */
				std::uint32_t color = tex.pixels[tx * tex.height + ty];
                *dest++ = lighting::apply(color, row_light);

				/* update world coords */
                fx += step_x;
                fy += step_y;
            }
        }
    }
}
