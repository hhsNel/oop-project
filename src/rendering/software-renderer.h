#pragma once
#define SOFTWARE_RENDERER_H

#include "rendering-backend.h"
#include "visplane.h"
#include "graphics/texture-manager.h"
#include "geometry/linedef.h"
#include "geometry/bsp-node.h"
#include "geometry/map-data.h"
#include "math/vec2.h"
#include <vector>
#include <cstddef>

namespace rendering {
    class software_renderer {
        rendering_backend *target;
        graphics::texture_manager const *tex_manager;
        geometry::map_data const *current_map;

		static constexpr float near_z = 0.1f;
		static constexpr float dl_start = 64.0f;
		static constexpr float dl_density = 0.25f;

        std::vector<int> upper_clip; 
        std::vector<int> lower_clip; 
		std::vector<visplane> visplanes;
		std::vector<float> euclidian_dist_factor;

		struct frame_rendering_data {
			math::vec2 cam_pos;
			float cam_height, cam_angle;
			unsigned int sw, sh;
			float half_sw;
			unsigned int pitch;
			std::uint32_t *__restrict mmio;
			float fov_scale;
			float inv_fov_scale;
			float cos_cam_angle, sin_cam_angle;
		};

		void render_bsp_node(util::indexed_storage<geometry::bsp_node>::id_t node_id, frame_rendering_data const frd);
        void project_and_draw_linedef(geometry::linedef const& line, frame_rendering_data const frd);
		void draw_solid_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const frd);
        void draw_portal_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const frd);
		void add_visplane(int x, int y_start, int y_end, float flat_height, graphics::texture_manager::texture_id tex_id, std::uint8_t const sector_light_level, frame_rendering_data const frd);
		void render_visplanes(frame_rendering_data const frd);
		bool is_box_visible(geometry::bsp_node::bounding_box const& box, frame_rendering_data const frd);
		__attribute__((always_inline)) inline int calculate_light(std::uint8_t const sector_light_level, float const depth);
		__attribute__((always_inline)) inline std::uint32_t apply_light(std::uint32_t const orig, int light);

    public:
        software_renderer();

        void set_target(rendering_backend *const tgt);
        void set_texture_manager(graphics::texture_manager const *const tm);
        void set_map(geometry::map_data const *const map);

        void render_bsp(geometry::bsp_node const* root, math::vec2 const cam_pos, float const cam_height, float cam_angle, float fov); 
    };
}
