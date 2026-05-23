#include "software-renderer.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

#include "lighting.h"
#include "geometry/sidedef.h"
#include "geometry/sector.h"
#include "geometry/subsector.h"

namespace rendering {

software_renderer::software_renderer(rendering_backend &tgt, assets::asset_manager const& tm, geometry::map_data const& map)
	: target(tgt), tex_manager(tm), current_map(map) {
}

void software_renderer::render_bsp_node(util::indexed_storage<geometry::bsp_node>::id_t node_id, frame_rendering_data const& frd) {
	/* if it's a leaf, then it's a subsector, so we should render the linedefs */
	if (geometry::bsp_node::is_leaf(node_id)) {
		auto subsector_id = geometry::bsp_node::get_id(node_id);

		geometry::subsector const& sub = current_map.subsectors[subsector_id];

		/* TODO: not sure if sprites should be queued first ? */
		std::uint8_t sub_light = 255;
		if (!sub.lines.empty()) {
			auto const& first_line = current_map.linedefs[sub.lines[0]];
			sub_light = current_map.sectors[current_map.sidedefs[first_line.front].facing_sector].light_level;
		}
		for(auto const& sprite : sub.sprites) {
			add_vissprite(sprite.get(), sub_light, frd);
		}

		for (auto const line_id : sub.lines) {
			project_and_draw_linedef(current_map.linedefs[line_id], frd);
		}
		return;
	}

	/* if not a leaf, then a bsp node */
	geometry::bsp_node const& node = current_map.nodes[node_id];

	/* check if camera is in front */
	bool is_front = node.is_pt_front_side(frd.cam_pos);

	/* identify closer and farther nodes and their bounding boxes */
	auto closer_node = is_front ? node.front : node.back;
	auto farther_node = is_front ? node.back : node.front;
//	auto closer_box = is_front ? node.front_box : node.back_box;
	auto farther_box = is_front ? node.back_box : node.front_box;

	/* visit the closer node first */
//	if (is_box_visible(closer_box, frd)) {
		render_bsp_node(closer_node, frd);
//	}

	/* visit the farther node second */
	if (is_box_visible(farther_box, frd)) {
		render_bsp_node(farther_node, frd);
	}
}

void software_renderer::project_and_draw_linedef(geometry::linedef line, frame_rendering_data const& frd) {
	math::vec2 tr_v1 = line("seg"_f)("point0"_f) - frd.cam_pos;
	math::vec2 tr_v2 = line("seg"_f)("point1"_f) - frd.cam_pos;
	
	tr_v1 = math::vec2::rotate_with_known_trig(tr_v1, frd.cos_cam_angle, -frd.sin_cam_angle);
	tr_v2 = math::vec2::rotate_with_known_trig(tr_v2, frd.cos_cam_angle, -frd.sin_cam_angle);

	/* now translated v1 & v2 are exactly opposite the camera */

	/* u coordinates for textures */
	float u1 = 0.0f;
	float u2 = line.len();

	/* return if behind camera */
	if (tr_v1("y"_f) <= near_z && tr_v2("y"_f) <= near_z) return;

	/* clip against the near plane */
	float inv_dy = 1.0f / (tr_v2("y"_f) - tr_v1("y"_f));
	if (tr_v1("y"_f) < near_z) {
		float t = (near_z - tr_v1("y"_f)) * inv_dy;
		tr_v1("x"_f) = tr_v1("x"_f) + t * (tr_v2("x"_f) - tr_v1("x"_f));
		tr_v1("y"_f) = near_z;
		u1 = u1 + t * (u2 - u1);
	} else if (tr_v2("y"_f) < near_z) {
		float t = (tr_v2("y"_f) - near_z) * inv_dy;
		tr_v2("x"_f) = tr_v2("x"_f) + t * (tr_v1("x"_f) - tr_v2("x"_f));
		tr_v2("y"_f) = near_z;
		u2 = u2 + t * (u1 - u2);
	}

	/* exact values for tex mapping */
	float proj_x1 = (tr_v1("x"_f) / tr_v1("y"_f)) * frd.fov_scale + frd.half_sw;
	float proj_x2 = (tr_v2("x"_f) / tr_v2("y"_f)) * frd.fov_scale + frd.half_sw;

	/* handle back-face viewing */
	if (proj_x1 > proj_x2) {
		/* cull back face (wall facing opposite side) */
		if (line.is_wall()) {
			return;
		}

		/* swap everything */
//		std::swap(proj_x1, proj_x2);
//		std::swap(z1, z2);
//		std::swap(u1, u2);
//		std::swap(line.front, line.back);
		/* TODO: CRITICAL */
	}

	if (line.is_wall()) {
		draw_solid_wall_span(proj_x1, proj_x2, tr_v1("y"_f), tr_v2("y"_f), u1, u2, line, frd);
	} else {
		draw_portal_wall_span(proj_x1, proj_x2, tr_v1("y"_f), tr_v2("y"_f), u1, u2, line, frd);
	}
}

void software_renderer::draw_solid_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const& frd) {
	/* useful stuff */
	geometry::sidedef const& sd = current_map.sidedefs[line.front];
	geometry::sector const& s = current_map.sectors[sd.facing_sector];
	assets::texture const& mt = tex_manager.wall_tx_by_id(sd.middle_tex); 

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
		unsigned int u = (unsigned int)(u_over_z * depth) % mt("width"_f);

		/* light depends on depth */
		int column_light = lighting::calculate(s.light_level, depth * (*frd.euclidian_dist_factor)[x]);

		/* project y onto the screen */
		int top_y = frd.sh/2 - (int)((s.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
		int bot_y = frd.sh/2 - (int)((s.floor_height - frd.cam_height) * frd.fov_scale * inv_z);

		/* crop to undendered space */
		int cropped_top_y = std::max(top_y, upper_clip[x]);
		int cropped_bot_y = std::min(bot_y, lower_clip[x]);

		/* draw ceiling from upper clip to wall top */
		visplane::add_column(visplanes, x, upper_clip[x], cropped_top_y, frd.sw, s.ceiling_height, s.ceiling_tex, s.light_level);
		/* draw floor from wall bottom to lower clip */
		visplane::add_column(visplanes, x, cropped_bot_y, lower_clip[x], frd.sw, s.floor_height, s.floor_tex, s.light_level);

		/* v scales linearly so pre-calculate step to avoid div */
		float v_step = (float)mt("height"_f) / (float)(bot_y - top_y);
		float current_v = (float)(cropped_top_y - top_y) * v_step;

		/* draw loop */
		for (int y = cropped_top_y; y < cropped_bot_y; ++y) {
			/* texture v coord */
			unsigned int v = static_cast<unsigned int>(current_v);
			current_v += v_step;

			/* write the pixel */
			std::uint32_t color = mt("pixels"_f)[u * mt("height"_f) + v];
			frd.mmio[x + y * frd.pitch] = lighting::apply(color, column_light);
		}

		/* update clipping */
		upper_clip[x] = frd.sh;
		lower_clip[x] = 0;
	}
}

void software_renderer::draw_portal_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const& frd) {
	/* useful stuff */
	geometry::sidedef const& front_sd = current_map.sidedefs[line.front];
	geometry::sidedef const& back_sd  = current_map.sidedefs[line.back];
	geometry::sector const& front = current_map.sectors[front_sd.facing_sector];
	geometry::sector const& back  = current_map.sectors[back_sd.facing_sector];
	assets::texture const& ut = tex_manager.wall_tx_by_id(front_sd.upper_tex);
	assets::texture const& lt = tex_manager.wall_tx_by_id(front_sd.lower_tex);
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
		unsigned int u_ut = u % ut("width"_f);
		unsigned int u_lt = u % lt("width"_f);

		/* light depends on depth */
		int column_light = lighting::calculate(front.light_level, depth * (*frd.euclidian_dist_factor)[x]);

		/* project front ceil, front floor, back ceil, back floor onto the screen */
		int c_fc = frd.sh/2 - (int)((front.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
		int c_ff = frd.sh/2 - (int)((front.floor_height - frd.cam_height) * frd.fov_scale * inv_z);
		int c_bc = frd.sh/2 - (int)((back.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z);
		int c_bf = frd.sh/2 - (int)((back.floor_height - frd.cam_height) * frd.fov_scale * inv_z);

		/* draw ceiling from upper clip to wall top */
		visplane::add_column(visplanes, x, upper_clip[x], std::max(c_fc, upper_clip[x]), frd.sw, front.ceiling_height, front.ceiling_tex, front.light_level);
		/* draw floor from wall bottom to lower clip */
		visplane::add_column(visplanes, x, std::min(c_ff, lower_clip[x]), lower_clip[x], frd.sw, front.floor_height, front.floor_tex, front.light_level);

		/* draw loop if the front ceil has lower y than the back ceil (is higher) */
		if (c_fc < c_bc) {
			int draw_top = std::max(c_fc, upper_clip[x]);
			int draw_bot = std::min(c_bc, lower_clip[x]);

			/* v scales linearly so pre-calculate step to avoid div */
			float v_step = (float)ut("height"_f) / (float)(c_bc - c_fc);
			float current_v = (float)(draw_top - c_fc) * v_step;

			for (int y = draw_top; y < draw_bot; ++y) {
				/* texture v coord */
				unsigned int v = static_cast<unsigned int>(current_v);
				current_v += v_step;

				/* write the pixel */
				std::uint32_t color = ut("pixels"_f)[u_ut * ut("height"_f) + v];
				frd.mmio[x + y * frd.pitch] = lighting::apply(color, column_light);
			}
		}

		/* same thing, but if the front floor is world lower than back floor (has higher y) */
		if (c_ff > c_bf) {
			int draw_top = std::max(c_bf, upper_clip[x]);
			int draw_bot = std::min(c_ff, lower_clip[x]);

			/* v scales linearly so pre-calculate step to avoid div */
			float v_step = (float)lt("height"_f) / (float)(c_ff - c_bf);
			float current_v = (float)(draw_top - c_bf) * v_step;

			for (int y = draw_top; y < draw_bot; ++y) {
				unsigned int v = static_cast<unsigned int>(current_v);
				current_v += v_step;

				std::uint32_t color = lt("pixels"_f)[u_lt * lt("height"_f) + v];
				frd.mmio[x + y * frd.pitch] = lighting::apply(color, column_light);
			}
		}

		/* update clipping */
		upper_clip[x] = std::max(upper_clip[x], std::max(c_fc, c_bc));
		lower_clip[x] = std::min(lower_clip[x], std::min(c_ff, c_bf));
	}
}

void software_renderer::render_visplanes(frame_rendering_data const& frd) {
	for (auto const& vp : visplanes)
		vp.render(tex_manager, frd);
	visplanes.clear();
}

void software_renderer::add_vissprite(sprite *const s, std::uint8_t light, frame_rendering_data const& frd) {
	math::vec2 tr_pos = (*s)("pos"_f) - frd.cam_pos;
	tr_pos = math::vec2::rotate_with_known_trig(tr_pos, frd.cos_cam_angle, -frd.sin_cam_angle);

	if(tr_pos("y"_f) <= near_z) return;

	float proj_x = (tr_pos("x"_f) / tr_pos("y"_f)) * frd.fov_scale + frd.half_sw;
	float scale = frd.fov_scale / tr_pos("y"_f) * s->inherent_scale;

	assets::texture const& tex = tex_manager.sprite_tx_by_id(s->tex_id);

	/* screen bounds */
	float half_width = (tex("width"_f) * scale) / 2.0f;
	int x1 = static_cast<int>(proj_x - half_width);
	int x2 = static_cast<int>(proj_x + half_width);

	/* frustum culling */
	if(x1 >= static_cast<int>(frd.sw) || x2 < 0) return;

	int cx1 = std::max(0, x1);
	int cx2 = std::min(x2, static_cast<int>(frd.sw) - 1);

	/* occlusion culling */
	bool visible = false;
	for(int x = cx1; x <= cx2; ++x) {
		if(lower_clip[x] > upper_clip[x]) {
			visible = true;
			break;
		}
	}
	if(!visible) return;

	std::vector<int> slice_uc(upper_clip.begin() + cx1, upper_clip.begin() + cx2 + 1);
	std::vector<int> slice_lc(lower_clip.begin() + cx1, lower_clip.begin() + cx2 + 1);
	vissprites.emplace_back(*s, tr_pos("y"_f), cx1, cx2, proj_x, scale, light, std::move(slice_uc), std::move(slice_lc));
}

void software_renderer::render_vissprites(frame_rendering_data const& frd) {
	vissprite::pa_sort(vissprites);

	for (auto const& vs : vissprites)
		vs.render(tex_manager, frd);

	vissprites.clear();
}

bool software_renderer::is_box_visible(geometry::bsp_node::bounding_box const& box, frame_rendering_data const& frd) {
	/* if the camera is inside, we def need to render the box */
	if (frd.cam_pos("x"_f) >= box.left   && frd.cam_pos("x"_f) <= box.right &&
		frd.cam_pos("y"_f) >= box.bottom && frd.cam_pos("y"_f) <= box.top) {
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

		bool p1_inside = p1("y"_f) >= near_z;
		bool p2_inside = p2("y"_f) >= near_z;

		if (p1_inside) {
			clipped[num_clipped++] = p1;
		}

		if (p1_inside != p2_inside) {
			/* edge intersects the near plane, calculate intersection */
			float t = (near_z - p1("y"_f)) / (p2("y"_f) - p1("y"_f));
			clipped[num_clipped++] = { p1("x"_f) + t * (p2("x"_f) - p1("x"_f)), near_z };
		}
	}

	/* entirely behind the near plane, cull */
	if (num_clipped == 0) return false;

	/* project the clipper verts onto the screen */
	float min_proj_x = 1e9f;
	float max_proj_x = -1e9f;
	for (int i = 0; i < num_clipped; ++i) {
		float proj_x = (clipped[i]("x"_f) / clipped[i]("y"_f)) * frd.fov_scale + frd.half_sw;
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

void software_renderer::render_bsp(math::vec2 const cam_pos, float const cam_height, float cam_angle, float fov) {

	if (current_map.root_node_id != util::indexed_storage<geometry::bsp_node>::nullid) {
		/* pre-calculate a bunch of things because calculating them per-pixel or even per-row is too slow */
		frame_rendering_data frd;
		frd.cam_pos	   = cam_pos;
		frd.cam_height	= cam_height;
		frd.cam_angle	 = cam_angle;
		frd.sw			= target("width"_f);
		frd.sh			= target("height"_f);
		frd.half_sw	   = frd.sw / 2.0f;
		frd.pitch		 = target("pitch"_f)/sizeof(std::uint32_t);
		frd.mmio		  = target("mmio"_f);
		frd.fov_scale	 = frd.half_sw / std::tan(fov/2.0f);
		frd.inv_fov_scale = 1.0f / frd.fov_scale;
		frd.cos_cam_angle = std::cos(frd.cam_angle);
		frd.sin_cam_angle = std::sin(frd.cam_angle);

		upper_clip.assign(frd.sw, 0);
		lower_clip.assign(frd.sw, frd.sh);

		if (euclidian_dist_factor.size() != frd.sw) {
			euclidian_dist_factor.resize(frd.sw);
			float center_x = frd.half_sw;
			for (unsigned int x = 0; x < frd.sw; ++x) {
				float dx = (x - center_x) * frd.inv_fov_scale;
				euclidian_dist_factor[x] = std::sqrt(dx * dx + 1.0f);
			}
		}
		frd.euclidian_dist_factor = &euclidian_dist_factor;

		render_bsp_node(current_map.root_node_id, frd);

		render_visplanes(frd);
		render_vissprites(frd);
	}
}

}
