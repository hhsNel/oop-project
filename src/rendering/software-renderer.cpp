#include "software-renderer.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

#include "geometry/sidedef.h"
#include "geometry/sector.h"
#include "geometry/subsector.h"

namespace rendering {

software_renderer::software_renderer() 
    : target(nullptr), tex_manager(nullptr), current_map(nullptr) {
}

void software_renderer::set_target(rendering_backend *const tgt) {
    this->target = tgt;
}

void software_renderer::set_texture_manager(graphics::texture_manager const *const tm) {
    this->tex_manager = tm;
}

void software_renderer::set_map(geometry::map_data const *const map) {
    this->current_map = map;
}

void software_renderer::render_bsp_node(util::indexed_storage<geometry::bsp_node>::id_t node_id, frame_rendering_data const frd) {
	/* if it's a leaf, then it's a subsector, so we should render the linedefs */
    if (node_id & geometry::bsp_node::leaf_flag) {
        auto subsector_id = node_id & ~geometry::bsp_node::leaf_flag;

        geometry::subsector const& sub = current_map->subsectors[subsector_id];

        for (auto line_id : sub.lines) {
            project_and_draw_linedef(current_map->linedefs[line_id], frd);
        }
        return;
    }

	/* if not a leaf, then a bsp node */
    geometry::bsp_node const& node = current_map->nodes[node_id];

	/* check if camera is in front */
    bool is_front = node.is_pt_front_side(frd.cam_pos);

    /* identify closer and farther nodes and their bounding boxes */
    auto closer_node = is_front ? node.front : node.back;
    auto farther_node = is_front ? node.back : node.front;
    auto closer_box = is_front ? node.front_box : node.back_box;
    auto farther_box = is_front ? node.back_box : node.front_box;

    /* visit the closer node first */
    if (is_box_visible(closer_box, frd)) {
        render_bsp_node(closer_node, frd);
    }

    /* visit the farther node second */
    if (is_box_visible(farther_box, frd)) {
        render_bsp_node(farther_node, frd);
    }
}

void software_renderer::project_and_draw_linedef(geometry::linedef const& line, frame_rendering_data const frd) {
    math::vec2 tr_v1 = line.v1 - frd.cam_pos;
    math::vec2 tr_v2 = line.v2 - frd.cam_pos;
    
    tr_v1 = math::vec2::rotate_with_known_trig(tr_v1, frd.cos_cam_angle, -frd.sin_cam_angle);
    tr_v2 = math::vec2::rotate_with_known_trig(tr_v2, frd.cos_cam_angle, -frd.sin_cam_angle);

	/* now translated v1 & v2 are exactly opposite the camera */

	/* u coordinates for textures */
    float u1 = 0.0f;
    float u2 = (line.v1 - line.v2).len();

	/* return if behind camera */
    if (tr_v1.y <= near_z && tr_v2.y <= near_z) return;

	/* clip against the near plane */
	float inv_dy = 1.0f / (tr_v2.y - tr_v1.y);
    if (tr_v1.y < near_z) {
        float t = (near_z - tr_v1.y) * inv_dy;
        tr_v1.x = tr_v1.x + t * (tr_v2.x - tr_v1.x);
        tr_v1.y = near_z;
        u1 = u1 + t * (u2 - u1);
    } else if (tr_v2.y < near_z) {
        float t = (tr_v2.y - near_z) * inv_dy;
        tr_v2.x = tr_v2.x + t * (tr_v1.x - tr_v2.x);
        tr_v2.y = near_z;
        u2 = u2 + t * (u1 - u2);
    }

	/* exact values for tex mapping */
    float proj_x1 = (tr_v1.x / tr_v1.y) * frd.fov_scale + frd.half_sw;
    float proj_x2 = (tr_v2.x / tr_v2.y) * frd.fov_scale + frd.half_sw;

	/* cull back face (wall facing opposite side */
    if (proj_x1 >= proj_x2) return;

	if (line.back == util::indexed_storage<geometry::sidedef>::nullid) {
        draw_solid_wall_span(proj_x1, proj_x2, tr_v1.y, tr_v2.y, u1, u2, line, frd);
    } else {
        draw_portal_wall_span(proj_x1, proj_x2, tr_v1.y, tr_v2.y, u1, u2, line, frd);
    }
}

void software_renderer::draw_solid_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const frd) {
	/* useful stuff */
    geometry::sidedef const& sd = current_map->sidedefs[line.front];
    geometry::sector const& s = current_map->sectors[sd.facing_sector];
    graphics::texture const& mt = tex_manager->wall_tx_by_id(sd.middle_tex); 

	/* bounds as integers */
    int x1 = static_cast<int>(proj_x1);
    int x2 = static_cast<int>(proj_x2);
	float inv_z1 = 1.0f / z1;
	float inv_z2 = 1.0f / z2;
	/* clamped x1 and x2 */
    int cx1 = std::max(0, x1);
    int cx2 = std::min((int)frd.sw, x2);

    for (int x = cx1; x < cx2; ++x) {
		/* done already */
        if (lower_clip[x] <= upper_clip[x]) continue;

		/* t E [0, 1) */
        float t = (x - proj_x1) / (proj_x2 - proj_x1);
		/* perspective correction with depth */
        float inv_z = (1.0f - t) * inv_z1 + t * inv_z2;
        float depth = 1.0f / inv_z;
        
		/* texture u coord */
        float u_over_z = (1.0f - t) * u1 * inv_z1 + t * u2 * inv_z2;
        unsigned int u = (unsigned int)(u_over_z * depth) % mt.width;

		/* light depends on depth */
		int column_light = calculate_light(s.light_level, depth * euclidian_dist_factor[x]);

		/* project y onto the screen */
        int top_y = frd.sh/2 - (int)((s.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
        int bot_y = frd.sh/2 - (int)((s.floor_height - frd.cam_height) * frd.fov_scale * inv_z);

		/* crop to undendered space */
        int cropped_top_y = std::max(top_y, upper_clip[x]);
        int cropped_bot_y = std::min(bot_y, lower_clip[x]);

		/* draw ceiling from upper clip to wall top */
        add_visplane(x, upper_clip[x], cropped_top_y, s.ceiling_height, s.ceiling_tex, s.light_level, frd);
		/* draw floor from wall bottom to lower clip */
        add_visplane(x, cropped_bot_y, lower_clip[x], s.floor_height, s.floor_tex, s.light_level, frd);

		/* v scales linearly so pre-calculate step to avoid div */
		float v_step = (float)mt.height / (float)(bot_y - top_y);
		float current_v = (float)(cropped_top_y - top_y) * v_step;

		/* draw loop */
        for (int y = cropped_top_y; y < cropped_bot_y; ++y) {
			/* texture v coord */
			unsigned int v = static_cast<unsigned int>(current_v);
			current_v += v_step;

			/* write the pixel */
            std::uint32_t color = mt.pixels[u * mt.height + v];
			frd.mmio[x + y * frd.pitch] = apply_light(color, column_light);
        }

		/* update clipping */
        upper_clip[x] = frd.sh;
        lower_clip[x] = 0;
    }
}

void software_renderer::draw_portal_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const frd) {
	/* useful stuff */
    geometry::sidedef const& front_sd = current_map->sidedefs[line.front];
    geometry::sidedef const& back_sd  = current_map->sidedefs[line.back];
    geometry::sector const& front = current_map->sectors[front_sd.facing_sector];
    geometry::sector const& back  = current_map->sectors[back_sd.facing_sector];
    graphics::texture const& ut = tex_manager->wall_tx_by_id(front_sd.upper_tex);
    graphics::texture const& lt = tex_manager->wall_tx_by_id(front_sd.lower_tex);
	float inv_z1 = 1.0f / z1;
	float inv_z2 = 1.0f / z2;

	/* bounds as integers */
    int x1 = (int)proj_x1;
    int x2 = (int)proj_x2;
	/* clamped x1 and x2 */
    int cx1 = std::max(0, x1);
    int cx2 = std::min((int)frd.sw, x2);

    for (int x = cx1; x < cx2; ++x) {
		/* done already */
        if (lower_clip[x] <= upper_clip[x]) continue;

		/* t E [0, 1) */
        float t = ((float)x - proj_x1) / (proj_x2 - proj_x1);
		/* perspective correction with depth */
        float inv_z = (1.0f - t) * inv_z1 + t * inv_z2;
        float depth = 1.0f / inv_z;
        
		/* texture u coord */
        float u_over_z = (1.0f - t) * u1 * inv_z1 + t * u2 * inv_z2;
        unsigned int u = (unsigned int)(u_over_z * depth);
		unsigned int u_ut = u % ut.width;
		unsigned int u_lt = u % lt.width;

		/* light depends on depth */
		int column_light = calculate_light(front.light_level, depth * euclidian_dist_factor[x]);

		/* project front ceil, front floor, back ceil, back floor onto the screen */
        int c_fc = frd.sh/2 - (int)((front.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
        int c_ff = frd.sh/2 - (int)((front.floor_height - frd.cam_height) * frd.fov_scale * inv_z);
        int c_bc = frd.sh/2 - (int)((back.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
        int c_bf = frd.sh/2 - (int)((back.floor_height - frd.cam_height) * frd.fov_scale * inv_z);

		/* draw ceiling from upper clip to wall top */
		add_visplane(x, upper_clip[x], std::max(c_fc, upper_clip[x]), front.ceiling_height, front.ceiling_tex, front.light_level, frd);
		/* draw floor from wall bottom to lower clip */
		add_visplane(x, std::min(c_ff, lower_clip[x]), lower_clip[x], front.floor_height, front.floor_tex, front.light_level, frd);

		/* draw loop if the front ceil has lower y than the back ceil (is higher) */
        if (c_fc < c_bc) {
            int draw_top = std::max(c_fc, upper_clip[x]);
            int draw_bot = std::min(c_bc, lower_clip[x]);

			/* v scales linearly so pre-calculate step to avoid div */
			float v_step = (float)ut.height / (float)(c_bc - c_fc);
			float current_v = (float)(draw_top - c_fc) * v_step;

            for (int y = draw_top; y < draw_bot; ++y) {
				/* texture v coord */
				unsigned int v = static_cast<unsigned int>(current_v);
				current_v += v_step;

				/* write the pixel */
				std::uint32_t color = ut.pixels[u_ut * ut.height + v];
                frd.mmio[x + y * frd.pitch] = apply_light(color, column_light);
            }
        }

		/* same thing, but if the front floor is world lower than back floor (has higher y) */
        if (c_ff > c_bf) {
            int draw_top = std::max(c_bf, upper_clip[x]);
            int draw_bot = std::min(c_ff, lower_clip[x]);

			/* v scales linearly so pre-calculate step to avoid div */
			float v_step = (float)lt.height / (float)(c_ff - c_bf);
			float current_v = (float)(draw_top - c_bf) * v_step;

            for (int y = draw_top; y < draw_bot; ++y) {
				unsigned int v = static_cast<unsigned int>(current_v);
				current_v += v_step;

				std::uint32_t color = lt.pixels[u_lt * lt.height + v];
                frd.mmio[x + y * frd.pitch] = apply_light(color, column_light);
            }
        }

		/* update clipping */
		upper_clip[x] = std::max(upper_clip[x], std::max(c_fc, c_bc));
        lower_clip[x] = std::min(lower_clip[x], std::min(c_ff, c_bf));
    }
}

void software_renderer::add_visplane(int x, int y_start, int y_end, float flat_height, graphics::texture_manager::texture_id tex_id, std::uint8_t const sector_light_level, frame_rendering_data const frd) {
    if (y_start >= y_end) return;

    visplane* target_vp = nullptr;

	/* try to find matching visplane */
    for (auto& vp : visplanes) {
        if (vp.height == flat_height && vp.tex_id == tex_id && vp.light_level == sector_light_level) {
            /* can merge if column empty or touches existing column */
            if (vp.top[x] == -1) {
                target_vp = &vp;
                break;
            } else if (y_start <= vp.bottom[x] && y_end >= vp.top[x]) {
                target_vp = &vp;
                break;
            }
        }
    }

	/* if none found, create a new visplane */
    if (!target_vp) {
        visplanes.emplace_back(x, frd.sw, flat_height, tex_id, sector_light_level);
        target_vp = &visplanes.back();
    }

    /* update the visplane bounds */
    target_vp->min_x = std::min(target_vp->min_x, x);
    target_vp->max_x = std::max(target_vp->max_x, x);

    if (target_vp->top[x] == -1) {
        target_vp->top[x] = y_start;
        target_vp->bottom[x] = y_end;
    } else {
        target_vp->top[x] = std::min(target_vp->top[x], y_start);
        target_vp->bottom[x] = std::max(target_vp->bottom[x], y_end);
    }
}

void software_renderer::render_visplanes(frame_rendering_data const frd) {
	/* optimizations */
    float dx_cos = frd.inv_fov_scale * frd.cos_cam_angle;
    float dx_sin = frd.inv_fov_scale * frd.sin_cam_angle;

    for (auto const& vp : visplanes) {
        graphics::texture const& tex = tex_manager->wall_tx_by_id(vp.tex_id);
        float h = std::abs(frd.cam_height - vp.height);

		/* true y bounds */
        int min_y = frd.sh, max_y = 0;
        for (int x = vp.min_x; x <= vp.max_x; ++x) {
            if (vp.top[x] != -1) {
                min_y = std::min(min_y, (int)vp.top[x]);
                max_y = std::max(max_y, (int)vp.bottom[x]);
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
            int row_light = calculate_light(vp.light_level, z);

			/* steps in world space as fxpt 16.16*/
            int32_t step_x = (int32_t)(z * dx_cos * 65536.0f);
            int32_t step_y = (int32_t)(z * dx_sin * 65536.0f);

			/* scan row to find spans */
            int x = vp.min_x;
            while (x <= vp.max_x) {
				/* record span */
                while (x <= vp.max_x && (vp.top[x] == -1 || y < vp.top[x] || y >= vp.bottom[x])) {
                    x++;
                }
                int span_start = x;

				/* find end of span */
                while (x <= vp.max_x && (vp.top[x] != -1 && y >= vp.top[x] && y < vp.bottom[x])) {
                    x++;
                }
                int span_end = x;

				/* span ended, draw */

                float px = (float)span_start - frd.half_sw;
				/* grab world space coords */
                float world_x_f = z * (px * frd.inv_fov_scale * frd.cos_cam_angle - frd.sin_cam_angle) + frd.cam_pos.x;
                float world_y_f = z * (px * frd.inv_fov_scale * frd.sin_cam_angle + frd.cos_cam_angle) + frd.cam_pos.y;

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
                    *dest++ = apply_light(color, row_light);

					/* update world coords */
                    fx += step_x;
                    fy += step_y;
                }
            }
        }
    }
    visplanes.clear();
}

bool software_renderer::is_box_visible(geometry::bsp_node::bounding_box const& box, frame_rendering_data const frd) {
	/* if the camera is inside, we def need to render the box */
    if (frd.cam_pos.x >= box.left   && frd.cam_pos.x <= box.right &&
        frd.cam_pos.y >= box.bottom && frd.cam_pos.y <= box.top) {
        return true;
    }

	/* corners of the bounding box */
    math::vec2 corners[4] = {
        {box.left, box.top},
        {box.right, box.top},
        {box.right, box.bottom},
        {box.left, box.bottom}
    };

	/* transform corners to camera space */
    for (int i = 0; i < 4; ++i) {
        corners[i] = corners[i] - frd.cam_pos;
        corners[i] = math::vec2::rotate_with_known_trig(corners[i], frd.cos_cam_angle, -frd.sin_cam_angle);
    }

	/* clipping a rectangle against a line produces at most 5 vertices */
    math::vec2 clipped[5];
    int num_clipped = 0;

    for (int i = 0; i < 4; ++i) {
        math::vec2 p1 = corners[i];
		/* modulo 4, but faster */
        math::vec2 p2 = corners[(i + 1) & 3];

        bool p1_inside = p1.y >= near_z;
        bool p2_inside = p2.y >= near_z;

        if (p1_inside) {
            clipped[num_clipped++] = p1;
        }

        if (p1_inside != p2_inside) {
            /* edge intersects the near plane, calculate intersection */
            float t = (near_z - p1.y) / (p2.y - p1.y);
            clipped[num_clipped++] = { p1.x + t * (p2.x - p1.x), near_z };
        }
    }

	/* entirely behind the near plane, cull */
    if (num_clipped == 0) return false;

    /* project the clipper verts onto the screen */
    float min_proj_x = 1e9f;
    float max_proj_x = -1e9f;
    for (int i = 0; i < num_clipped; ++i) {
        float proj_x = (clipped[i].x / clipped[i].y) * frd.fov_scale + frd.half_sw;
        min_proj_x = std::min(min_proj_x, proj_x);
        max_proj_x = std::max(max_proj_x, proj_x);
    }

    int min_x = static_cast<int>(min_proj_x);
    int max_x = static_cast<int>(max_proj_x);

    /* frustum culling (is outside of FOV?) */
    if (min_x >= static_cast<int>(frd.sw) || max_x < 0) {
        return false;
    }

	/* clamp to the parts visible on screen */
    int cx1 = std::max(0, min_x);
    int cx2 = std::min(static_cast<int>(frd.sw) - 1, max_x);

    /* occlusion culling (fully behind walls) */
    for (int x = cx1; x <= cx2; ++x) {
        if (lower_clip[x] > upper_clip[x]) {
			 /* at least one column is open, so it's partially visible */
            return true;
        }
    }

    /* fully occluded by walls */
    return false;
}

__attribute__((always_inline)) inline int software_renderer::calculate_light(std::uint8_t const sector_light_level, float const depth) {
	if(depth > dl_start) {
		int corrected_light = sector_light_level - (depth - dl_start) * dl_density;
		if(corrected_light < 0) corrected_light = 0;
		return corrected_light;
	} else {
		return sector_light_level;
	}
}

__attribute__((always_inline)) inline std::uint32_t software_renderer::apply_light(std::uint32_t const orig, int light) {
	std::uint32_t swar_rb = orig & 0x00ff00ff;
	std::uint32_t swar_g  = orig & 0x0000ff00;

	swar_rb = ((swar_rb * light) >> 8) & 0x00ff00ff;
	swar_g  = ((swar_g  * light) >> 8) & 0x0000ff00;

	return swar_rb | swar_g;
}

void software_renderer::render_bsp(geometry::bsp_node const* /*root*/, math::vec2 const cam_pos, float const cam_height, float cam_angle, float fov) {

    if (current_map && current_map->root_node_id != util::indexed_storage<geometry::bsp_node>::nullid) {
		/* pre-calculate a bunch of things because calculating them per-pixel or even per-row is too slow */
		frame_rendering_data frd;
		frd.cam_pos       = cam_pos;
		frd.cam_height    = cam_height;
		frd.cam_angle     = cam_angle;
		frd.sw            = target->get_width();
		frd.sh            = target->get_height();
		frd.half_sw       = frd.sw / 2.0f;
		frd.pitch         = target->get_pitch()/sizeof(std::uint32_t);
		frd.mmio          = target->get_mmio();
		frd.fov_scale     = frd.half_sw / std::tan(fov/2.0f);
		frd.inv_fov_scale = 1.0f / frd.fov_scale;
		frd.cos_cam_angle = std::cos(frd.cam_angle);
		frd.sin_cam_angle = std::sin(frd.cam_angle);

		upper_clip.resize(frd.sw);
		lower_clip.resize(frd.sw);

		for(auto& i : upper_clip) i = 0;
		for(auto& i : lower_clip) i = frd.sh;

		if (euclidian_dist_factor.size() != frd.sw) {
			euclidian_dist_factor.resize(frd.sw);
			float center_x = frd.half_sw;
			for (unsigned int x = 0; x < frd.sw; ++x) {
				float dx = (x - center_x) * frd.inv_fov_scale;
				euclidian_dist_factor[x] = std::sqrt(dx * dx + 1.0f);
			}
		}

        render_bsp_node(current_map->root_node_id, frd);

		render_visplanes(frd);
    }
}

}

