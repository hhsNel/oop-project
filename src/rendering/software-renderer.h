#pragma once
#define SOFTWARE_RENDERER_H

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
        rendering_backend &target;
        assets::asset_manager const& tex_manager;
        geometry::map_data const& current_map;

        static constexpr float near_z = 0.1f;

        std::vector<int> upper_clip;
        std::vector<int> lower_clip;
        std::vector<visplane> visplanes;
        std::vector<vissprite> vissprites;

        std::vector<float> euclidian_dist_factor;

        void render_bsp_node(util::indexed_storage<geometry::bsp_node>::id_t node_id, frame_rendering_data const& frd);
        void project_and_draw_linedef(geometry::linedef line, frame_rendering_data const& frd);
        void draw_solid_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const& frd);
        void draw_portal_wall_span(float proj_x1, float proj_x2, float z1, float z2, float u1, float u2, geometry::linedef const& line, frame_rendering_data const& frd);
        void render_visplanes(frame_rendering_data const& frd);
        void add_vissprite(sprite *const s, std::uint8_t light, frame_rendering_data const& frd);
        void render_vissprites(frame_rendering_data const& frd);
        bool is_box_visible(geometry::bsp_node::bounding_box const& box, frame_rendering_data const& frd);

    public:
        software_renderer(rendering_backend &tgt, assets::asset_manager const& tm, geometry::map_data const& map);

        void render_bsp(math::vec2 const cam_pos, float const cam_height, float cam_angle, float fov);
    };
}
