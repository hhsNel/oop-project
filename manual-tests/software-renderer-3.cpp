/* similiar to test 2 , 0xffffff*/

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
	r2d.draw_text(ss.str().c_str(), 10, 10, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "Raw Render: " << raw_render_ms << " ms";
	r2d.draw_text(ss.str().c_str(), 10, 30, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "Total Time: " << total_time_ms / 1000.0f << " s";
	r2d.draw_text(ss.str().c_str(), 10, 50, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "FPS: " << current_fps;
	r2d.draw_text(ss.str().c_str(), 10, 70, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "Avg FPS: " << avg_fps;
	r2d.draw_text(ss.str().c_str(), 10, 90, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "Camera: (" << cam_pos("x"_f) << ", " << cam_pos("y"_f) << ", " << cam_height << ")";
	r2d.draw_text(ss.str().c_str(), 10, 110, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "Camera Angle: " << cam_angle << "rad";
	r2d.draw_text(ss.str().c_str(), 10, 130, 12, 16, 0xffffff);
	ss.str(""); ss.clear();
	ss << "FOV: " << fov << "rad";
	r2d.draw_text(ss.str().c_str(), 10, 150, 12, 16, 0xffffff);

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

    cam_pos("x"_f)  = std::get<0>(prev)("x"_f) * (1.0f - t) + std::get<0>(next)("x"_f) * t;
    cam_pos("y"_f)  = std::get<0>(prev)("y"_f) * (1.0f - t) + std::get<0>(next)("y"_f) * t;
    cam_height = std::get<1>(prev)   * (1.0f - t) + std::get<1>(next)   * t;
    cam_angle  = std::get<2>(prev)   * (1.0f - t) + std::get<2>(next)   * t;
    fov        = std::get<3>(prev)   * (1.0f - t) + std::get<3>(next)   * t;
}

int main() {
    std::cout << "Starting Software Renderer PoC..." << std::endl;

    auto backend = std::make_unique<rendering::drm_kms::backend>();
    if (backend->bad()) {
        std::cerr << "Failed to initialize DRM/KMS backend." << std::endl;
        return -1;
    }

    auto modes = (*backend)("modes"_f);
    if (!modes.empty()) {
        backend->push_mode(std::move(modes[0]));
    }


	util::resource_loader rl;
    auto tm = std::make_unique<assets::asset_manager>(assets::asset_manager::load(rl));

	rendering::renderer_2d r2d(*backend.get(), *tm.get(), tm->ui_tx_by_id(0));

    geometry::map_data map;
    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    auto s1_id = map.sectors.add(geometry::sector(0.0f, 256.0f, 0, 0, 192));

    auto s2_id = map.sectors.add(geometry::sector(32.0f, 224.0f, 1, 1, 255));

    auto sd_r1_solid_id = map.sidedefs.add(geometry::sidedef(s1_id, -1, 0, -1));

    auto sd_r2_solid_id = map.sidedefs.add(geometry::sidedef(s2_id, -1, 1, -1));

    auto sd_portal_front_id = map.sidedefs.add(geometry::sidedef(s1_id, 0, -1, 0));

    auto sd_portal_back_id = map.sidedefs.add(geometry::sidedef(s2_id, 1, -1, 1));

    auto sd_pillar_id = map.sidedefs.add(geometry::sidedef(s1_id, -1, 2, -1));


	auto add_line = [&](float x1, float y1, float x2, float y2, auto f, auto b) {
		return map.linedefs.add(geometry::linedef({x1, y1}, {x2, y2}, f, b));
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

    geometry::subsector ss_south({ld_w_s, ld_s_s, ld_e_s, ld_p_s});
    auto ss_south_id = map.subsectors.add(std::move(ss_south));

    geometry::subsector ss_north({ld_w_n, ld_n_n, ld_e_n, ld_p_n});
    auto ss_north_id = map.subsectors.add(std::move(ss_north));

    geometry::subsector ss_west({ld_w_w, ld_p_w});
    auto ss_west_id = map.subsectors.add(std::move(ss_west));

    geometry::subsector ss_east({ld_e_e, ld_p_e});
    auto ss_east_id = map.subsectors.add(std::move(ss_east));

    geometry::subsector ss_pillar;
    auto ss_pillar_id = map.subsectors.add(std::move(ss_pillar));

    geometry::subsector ss2({ld1_r2, ld4, ld5, ld6});
    auto ss2_id = map.subsectors.add(std::move(ss2));

    /* BSP TREE */
    geometry::bsp_node n_east_west(
        {576.0f, 0.0f},
        {0.0f, 1024.0f},
        geometry::bsp_node::bounding_box{576.0f, 448.0f, 576.0f, 1024.0f},
        geometry::bsp_node::bounding_box{576.0f, 448.0f, 448.0f, 576.0f},
        0x80000000 | ss_east_id,
        0x80000000 | ss_pillar_id
    );
    auto n_east_west_id = map.nodes.add(n_east_west);

    geometry::bsp_node n_west_center(
        {448.0f, 0.0f},
        {0.0f, 1024.0f},
        geometry::bsp_node::bounding_box{576.0f, 448.0f, 448.0f, 1024.0f},
        geometry::bsp_node::bounding_box{576.0f, 448.0f, 0.0f, 448.0f},
        n_east_west_id,
        0x80000000 | ss_west_id
    );
    auto n_west_center_id = map.nodes.add(n_west_center);

    geometry::bsp_node n_north_split(
        {0.0f, 576.0f},
        {1024.0f, 0.0f},
        geometry::bsp_node::bounding_box{576.0f, 448.0f, 0.0f, 1024.0f},
        geometry::bsp_node::bounding_box{1024.0f, 576.0f, 0.0f, 1024.0f},
        n_west_center_id,
        0x80000000 | ss_north_id
    );
    auto n_north_split_id = map.nodes.add(n_north_split);

    geometry::bsp_node n_south_split(
        {0.0f, 448.0f},
        {1024.0f, 0.0f},
        geometry::bsp_node::bounding_box{448.0f, 0.0f, 0.0f, 1024.0f},
        geometry::bsp_node::bounding_box{1024.0f, 448.0f, 0.0f, 1024.0f},
        0x80000000 | ss_south_id,
        n_north_split_id
    );
    auto n_south_split_id = map.nodes.add(n_south_split);

    geometry::bsp_node root_node(
        {0.0f, 1024.0f},
        {1024.0f, 0.0f},
        geometry::bsp_node::bounding_box{1024.0f, 0.0f, 0.0f, 1024.0f},
        geometry::bsp_node::bounding_box{2048.0f, 1024.0f, 0.0f, 1024.0f},
        n_south_split_id,
        0x80000000 | ss2_id
    );

    auto root_node_id = map.nodes.add(root_node);
    map.root_node_id = root_node_id;

    rendering::software_renderer renderer(*backend.get(), *tm.get(), map);

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
