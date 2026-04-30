/* similiar to test 2 */

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "rendering/drm-kms/backend.h"
#include "rendering/software-renderer.h"
#include "geometry/map-data.h"
#include "geometry/sector.h"
#include "geometry/sidedef.h"
#include "geometry/linedef.h"
#include "geometry/subsector.h"
#include "geometry/bsp-node.h"
#include "rendering/sprite.h"
#include "rendering/renderer-2d.h"

void render_frame(math::vec2 cam_pos, float cam_height, float &cam_angle, float fov,
                  rendering::rendering_backend *backend, rendering::software_renderer &renderer, rendering::renderer_2d &r2d,
                  float dt_ms, float total_time_ms, int frame_count) {

	auto raw_start = std::chrono::high_resolution_clock::now();

	renderer.render_bsp(cam_pos, cam_height, cam_angle, fov);

	auto raw_end = std::chrono::high_resolution_clock::now();
	float raw_render_ms = std::chrono::duration<float, std::milli>(raw_end - raw_start).count();

	int current_fps = dt_ms > 0.0f ? static_cast<int>(1000.0f / dt_ms) : 0;
	int avg_fps = total_time_ms > 0.0f ? static_cast<int>(frame_count / (total_time_ms / 1000.0f)) : 0;

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2);

	ss << "Frame Time: " << dt_ms << " ms";
	r2d.draw_text(ss.str().c_str(), 10, 10, 12, 16);
	ss.str(""); ss.clear();
	ss << "Raw Render: " << raw_render_ms << " ms";
	r2d.draw_text(ss.str().c_str(), 10, 30, 12, 16);
	ss.str(""); ss.clear();
	ss << "Total Time: " << total_time_ms / 1000.0f << " s";
	r2d.draw_text(ss.str().c_str(), 10, 50, 12, 16);
	ss.str(""); ss.clear();
	ss << "FPS: " << current_fps;
	r2d.draw_text(ss.str().c_str(), 10, 70, 12, 16);
	ss.str(""); ss.clear();
	ss << "Avg FPS: " << avg_fps;
	r2d.draw_text(ss.str().c_str(), 10, 90, 12, 16);
	ss.str(""); ss.clear();
	ss << "Camera: (" << cam_pos.x << ", " << cam_pos.y << ", " << cam_height << ")";
	r2d.draw_text(ss.str().c_str(), 10, 110, 12, 16);
	ss.str(""); ss.clear();
	ss << "Camera Angle: " << cam_angle << "rad";
	r2d.draw_text(ss.str().c_str(), 10, 130, 12, 16);
	ss.str(""); ss.clear();
	ss << "FOV: " << fov << "rad";
	r2d.draw_text(ss.str().c_str(), 10, 150, 12, 16);

	backend->flush();
	backend->wait_for_vsync();
}

void move_camera(math::vec2 &cam_pos, float &cam_height, float &cam_angle, float &fov, int frame_count, int max_frame) {
    std::vector<std::tuple<math::vec2, float, float, float>> arr = {
		std::make_tuple(math::vec2(128.0f, 128.0f),  64.0f,  M_PI/180*45.0f,  M_PI/180*90.0f),
		std::make_tuple(math::vec2(512.0f, 896.0f),  64.0f,  M_PI/180*90.0f,  M_PI/180*90.0f),
		std::make_tuple(math::vec2(896.0f, 512.0f),  64.0f,  M_PI/180*135.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(896.0f, 896.0f),  32.0f,  M_PI/180*135.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(128.0f, 896.0f),  64.0f,  M_PI/180*45.0f,  M_PI/180*90.0f),
		std::make_tuple(math::vec2(128.0f, 896.0f),  64.0f,  M_PI/180*270.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(512.0f, 896.0f),  64.0f,  M_PI/180*90.0f,  M_PI/180*120.0f),
		std::make_tuple(math::vec2(512.0f, 1152.0f), 96.0f,  M_PI/180*90.0f,  M_PI/180*90.0f),
		std::make_tuple(math::vec2(512.0f, 1536.0f), 96.0f,  M_PI/180*90.0f,  M_PI/180*90.0f),
		std::make_tuple(math::vec2(512.0f, 1536.0f), 96.0f,  M_PI/180*180.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(128.0f, 1920.0f), 96.0f,  M_PI/180*315.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(896.0f, 1920.0f), 160.0f, M_PI/180*225.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(896.0f, 1920.0f), 160.0f, M_PI/180*225.0f, M_PI/180*45.0f),
		std::make_tuple(math::vec2(512.0f, 1152.0f), 96.0f,  M_PI/180*270.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(512.0f, 896.0f),  64.0f,  M_PI/180*270.0f, M_PI/180*90.0f),
		std::make_tuple(math::vec2(128.0f, 128.0f),  64.0f,  M_PI/180*225.0f, M_PI/180*90.0f)
    };

    int num_segments = arr.size() - 1;

    float progress = static_cast<float>(frame_count) / static_cast<float>(max_frame > 1 ? max_frame - 1 : 1);

    float scaled_progress = progress * num_segments;

    int segment = static_cast<int>(scaled_progress);

    float t = scaled_progress - segment;

    if (segment >= num_segments) {
        segment = num_segments - 1;
        t = 1.0f;
    }

    std::tuple<math::vec2, float, float, float> prev = arr[segment];
    std::tuple<math::vec2, float, float, float> next = arr[segment + 1];

    cam_pos.x  = std::get<0>(prev).x * (1.0f - t) + std::get<0>(next).x * t;
    cam_pos.y  = std::get<0>(prev).y * (1.0f - t) + std::get<0>(next).y * t;
    cam_height = std::get<1>(prev)   * (1.0f - t) + std::get<1>(next)   * t;
    cam_angle  = std::get<2>(prev)   * (1.0f - t) + std::get<2>(next)   * t;
    fov        = std::get<3>(prev)   * (1.0f - t) + std::get<3>(next)   * t;
}

int main() {
    std::cout << "Starting Software Renderer PoC..." << std::endl;

    auto backend = std::make_unique<rendering::drm_kms::backend>();
    if (backend->is_bad()) {
        std::cerr << "Failed to initialize DRM/KMS backend." << std::endl;
        return -1;
    }

    auto modes = backend->get_modes();
    if (!modes.empty()) {
        backend->set_mode(std::move(modes[0]));
    }

    rendering::software_renderer renderer;
    renderer.set_target(backend.get());

    auto tm = std::make_unique<graphics::texture_manager>(graphics::texture_manager::load());
    renderer.set_texture_manager(tm.get());

	rendering::renderer_2d r2d;
    r2d.set_target(backend.get());
    r2d.set_texture_manager(tm.get());
    const graphics::texture& atlas = tm->flat_tx_by_id(0);
    r2d.set_font_texture(&atlas);

    geometry::map_data map;
    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    geometry::sector sector1;
    sector1.floor_height = 0.0f;
    sector1.ceiling_height = 256.0f;
    sector1.light_level = 192;
	sector1.floor_tex = 0;
	sector1.ceiling_tex = 0;
    auto s1_id = map.sectors.add(sector1);

    geometry::sector sector2;
    sector2.floor_height = 32.0f;
    sector2.ceiling_height = 224.0f;
    sector2.light_level = 255; 
	sector2.floor_tex = 1;
	sector2.ceiling_tex = 1;
    auto s2_id = map.sectors.add(sector2);

    geometry::sidedef sd_r1_solid;
    sd_r1_solid.facing_sector = s1_id;
    sd_r1_solid.middle_tex = 0;
    auto sd_r1_solid_id = map.sidedefs.add(sd_r1_solid);

    geometry::sidedef sd_r2_solid;
    sd_r2_solid.facing_sector = s2_id;
    sd_r2_solid.middle_tex = 1;
    auto sd_r2_solid_id = map.sidedefs.add(sd_r2_solid);

    geometry::sidedef sd_portal_front;
    sd_portal_front.facing_sector = s1_id;
    sd_portal_front.upper_tex = 0;
    sd_portal_front.lower_tex = 0;
    auto sd_portal_front_id = map.sidedefs.add(sd_portal_front);

    geometry::sidedef sd_portal_back;
    sd_portal_back.facing_sector = s2_id;
    sd_portal_back.upper_tex = 1;
    sd_portal_back.lower_tex = 1;
    auto sd_portal_back_id = map.sidedefs.add(sd_portal_back);

    geometry::sidedef sd_pillar;
    sd_pillar.facing_sector = s1_id;
    sd_pillar.middle_tex = 2;
    auto sd_pillar_id = map.sidedefs.add(sd_pillar);

    auto add_line = [&](float x1, float y1, float x2, float y2, auto f, auto b) {
        geometry::linedef ld;
        ld.v1 = {x1, y1};
        ld.v2 = {x2, y2};
        ld.front = f;
        ld.back = b;
        return map.linedefs.add(ld);
    };

    auto ld_w_s = add_line(0.0f, 0.0f, 0.0f, 448.0f, sd_r1_solid_id, null_sd);
    auto ld_w_w = add_line(0.0f, 448.0f, 0.0f, 576.0f, sd_r1_solid_id, null_sd);
    auto ld_w_n = add_line(0.0f, 576.0f, 0.0f, 1024.0f, sd_r1_solid_id, null_sd);

    auto ld_n_n = add_line(0.0f, 1024.0f, 1024.0f, 1024.0f, sd_portal_front_id, sd_portal_back_id);

    auto ld_e_n = add_line(1024.0f, 1024.0f, 1024.0f, 576.0f, sd_r1_solid_id, null_sd);
    auto ld_e_e = add_line(1024.0f, 576.0f, 1024.0f, 448.0f, sd_r1_solid_id, null_sd);
    auto ld_e_s = add_line(1024.0f, 448.0f, 1024.0f, 0.0f, sd_r1_solid_id, null_sd);

    auto ld_s_s = add_line(1024.0f, 0.0f, 0.0f, 0.0f, sd_r1_solid_id, null_sd);

    auto ld_p_s = add_line(576.0f, 448.0f, 448.0f, 448.0f, sd_pillar_id, null_sd);
    auto ld_p_n = add_line(576.0f, 576.0f, 448.0f, 576.0f, sd_pillar_id, null_sd);
    auto ld_p_w = add_line(448.0f, 576.0f, 448.0f, 448.0f, sd_pillar_id, null_sd);
    auto ld_p_e = add_line(576.0f, 448.0f, 576.0f, 576.0f, sd_pillar_id, null_sd);

    auto ld4 = add_line(1024.0f, 2048.0f, 1024.0f, 1024.0f, sd_r2_solid_id, null_sd); 
    auto ld5 = add_line(0.0f, 2048.0f, 1024.0f, 2048.0f, sd_r2_solid_id, null_sd);    
    auto ld6 = add_line(0.0f, 1024.0f, 0.0f, 2048.0f, sd_r2_solid_id, null_sd);       
    auto ld1_r2 = add_line(1024.0f, 1024.0f, 0.0f, 1024.0f, sd_portal_back_id, sd_portal_front_id);

    geometry::subsector ss_south;
    ss_south.lines = {ld_w_s, ld_s_s, ld_e_s, ld_p_s};
    
    auto spr0 = std::make_unique<rendering::sprite>();
    spr0->pos = { 128.0f, 256.0f };
	spr0->z_pos = -256.0f;
    spr0->tex_id = 0;
	spr0->inherent_scale = 0.25;
    ss_south.sprites.push_back(std::move(spr0));
    
    auto ss_south_id = map.subsectors.add(std::move(ss_south));

    geometry::subsector ss_north;
    ss_north.lines = {ld_w_n, ld_n_n, ld_e_n, ld_p_n};
    auto ss_north_id = map.subsectors.add(std::move(ss_north));

    geometry::subsector ss_west;
    ss_west.lines = {ld_w_w, ld_p_w};
    auto ss_west_id = map.subsectors.add(std::move(ss_west));

    geometry::subsector ss_east;
    ss_east.lines = {ld_e_e, ld_p_e};
    auto ss_east_id = map.subsectors.add(std::move(ss_east));

    geometry::subsector ss_pillar;
    ss_pillar.lines = {};
    auto ss_pillar_id = map.subsectors.add(std::move(ss_pillar));

    geometry::subsector ss2;
    ss2.lines = {ld1_r2, ld4, ld5, ld6};

    auto spr1 = std::make_unique<rendering::sprite>();
    spr1->pos = { 768.0f, 1536.0f };
	spr1->z_pos = -256.0f;
    spr1->tex_id = 1;
	spr1->inherent_scale = 0.25;
    ss2.sprites.push_back(std::move(spr1));

    auto ss2_id = map.subsectors.add(std::move(ss2));

    geometry::bsp_node n_east_west;
    n_east_west.pl_coord = {576.0f, 0.0f};
    n_east_west.pl_dir = {0.0f, 1024.0f};
    n_east_west.front_box = {576.0f, 448.0f, 576.0f, 1024.0f};
    n_east_west.back_box  = {576.0f, 448.0f, 448.0f, 576.0f};
    n_east_west.front = geometry::bsp_node::leaf_flag | ss_east_id;
    n_east_west.back = geometry::bsp_node::leaf_flag | ss_pillar_id;
    auto n_east_west_id = map.nodes.add(n_east_west);

    geometry::bsp_node n_west_center;
    n_west_center.pl_coord = {448.0f, 0.0f};
    n_west_center.pl_dir = {0.0f, 1024.0f};
    n_west_center.front_box = {576.0f, 448.0f, 448.0f, 1024.0f};
    n_west_center.back_box  = {576.0f, 448.0f, 0.0f, 448.0f};
    n_west_center.front = n_east_west_id;
    n_west_center.back = geometry::bsp_node::leaf_flag | ss_west_id;
    auto n_west_center_id = map.nodes.add(n_west_center);

    geometry::bsp_node n_north_split;
    n_north_split.pl_coord = {0.0f, 576.0f};
    n_north_split.pl_dir = {1024.0f, 0.0f};
    n_north_split.front_box = {576.0f, 448.0f, 0.0f, 1024.0f};
    n_north_split.back_box  = {1024.0f, 576.0f, 0.0f, 1024.0f};
    n_north_split.front = n_west_center_id;
    n_north_split.back = geometry::bsp_node::leaf_flag | ss_north_id;
    auto n_north_split_id = map.nodes.add(n_north_split);

    geometry::bsp_node n_south_split;
    n_south_split.pl_coord = {0.0f, 448.0f};
    n_south_split.pl_dir = {1024.0f, 0.0f};
    n_south_split.front_box = {448.0f, 0.0f, 0.0f, 1024.0f};
    n_south_split.back_box  = {1024.0f, 448.0f, 0.0f, 1024.0f};
    n_south_split.front = geometry::bsp_node::leaf_flag | ss_south_id;
    n_south_split.back = n_north_split_id;
    auto n_south_split_id = map.nodes.add(n_south_split);

    geometry::bsp_node root_node;
    root_node.pl_coord = {0.0f, 1024.0f};
    root_node.pl_dir = {1024.0f, 0.0f};
    root_node.front_box = {1024.0f, 0.0f, 0.0f, 1024.0f};
    root_node.back_box  = {2048.0f, 1024.0f, 0.0f, 1024.0f};
    root_node.front = n_south_split_id;
    root_node.back = geometry::bsp_node::leaf_flag | ss2_id;

    auto root_node_id = map.nodes.add(root_node);
    map.root_node_id = root_node_id;

    renderer.set_map(&map);

    math::vec2 cam_pos(512.0f, 512.0f);
    float cam_height = 128.0f;
    float cam_angle = 0.0f;
    float fov = 90.0f;

	auto start_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = start_time;
	float total_time_ms;
    for(int i = 0; i < 4096; ++i) {
		move_camera(cam_pos, cam_height, cam_angle, fov, i, 4096);
		auto current_time = std::chrono::high_resolution_clock::now();
        float dt_ms = std::chrono::duration<float, std::milli>(current_time - last_frame_time).count();
        total_time_ms = std::chrono::duration<float, std::milli>(current_time - start_time).count();
        last_frame_time = current_time;
		render_frame(cam_pos, cam_height, cam_angle, fov, backend.get(), renderer, r2d, dt_ms, total_time_ms, i);
    }

	int avg_fps = total_time_ms > 0.0f ? static_cast<int>(4096 / (total_time_ms / 1000.0f)) : 0;
	std::cout << "Average FPS: " << avg_fps << std::endl;
    return 0;
}
