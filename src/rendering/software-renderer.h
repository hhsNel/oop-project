#pragma once

#include "rendering-backend.h"
#include "frd.h"
#include "visplane.h"
#include "vissprite.h"
#include "assets/asset-manager.h"
#include "geometry/linedef.h"
#include "geometry/bsp-node.h"
#include "geometry/map-data.h"
#include "math/vec2.h"
#include <vector>
#include <cstddef>

namespace rendering {
	class software_renderer {
		/* chosen backend */
		rendering_backend &target;
		/* texture mgr reference */
		assets::asset_manager const& tex_manager;
		/* map reference */
		geometry::map_data const& current_map;

		/* near clipping plane distance */
		static constexpr float near_z = 0.1f;

		/* upper clip array */
		std::vector<int> upper_clip;
		/* lower clip array */
		std::vector<int> lower_clip;
		/* rendered visplanes */
		std::vector<visplane> visplanes;
		/* rendered vissprites */
		std::vector<vissprite> vissprites;

		/* euclidian distance correction */
		std::vector<float> euclidian_dist_factor;

		/* recursively render a BSP node */
		void render_bsp_node(util::indexed_storage<geometry::bsp_node>::id_t
			node_id, frame_rendering_data const& frd);
		/* render a linedef */
		void project_and_draw_linedef(geometry::linedef line,
			frame_rendering_data const& frd);
		/* render a wall linedef */
		void draw_solid_wall_span(float proj_x1, float proj_x2, float z1,
			float z2, float u1, float u2, geometry::linedef const& line,
			frame_rendering_data const& frd);
		/* render a portal linedef */
		void draw_portal_wall_span(float proj_x1, float proj_x2, float z1,
			float z2, float u1, float u2, geometry::linedef const& line,
			frame_rendering_data const& frd);
		/* render all visplanes */
		void render_visplanes(frame_rendering_data const& frd);
		/* queue a vissprite to be rendered */
		void add_vissprite(sprite *const s, std::uint8_t light,
				frame_rendering_data const& frd);
		/* render all vissprites */
		void render_vissprites(frame_rendering_data const& frd);
		/* whether or not a BSP should be culled based on its bounding box */
		bool is_box_visible(geometry::bsp_node::bounding_box const& box,
				frame_rendering_data const& frd);

	public:
		/* constructor */
		software_renderer(rendering_backend &tgt, assets::asset_manager const& tm, geometry::map_data const& map);

		/* render whole bsp tree */
		void render_bsp(math::vec2 const cam_pos, float const cam_height, float cam_angle, float fov);
	};
}
