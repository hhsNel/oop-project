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
        std::make_tuple(math::vec2(256,256),   64,  0,       M_PI/180*90.0f),
        std::make_tuple(math::vec2(1024,1024), 192, M_PI/2,  M_PI/180*90.0f),
        std::make_tuple(math::vec2(2000,2000), 128, -M_PI/2, M_PI/180*90.0f),
        std::make_tuple(math::vec2(1024,1024), 64,  M_PI,    M_PI/180*90.0f),
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

    geometry::map_data room_map;

    geometry::sector room_sector;
    room_sector.floor_height = 0;
    room_sector.ceiling_height = 256;
    room_sector.light_level = 255;
	room_sector.floor_tex = 0;
	room_sector.ceiling_tex = 0;
    auto sector_id = room_map.sectors.add(room_sector);

    geometry::sidedef room_sidedef;
    room_sidedef.facing_sector = sector_id;
    room_sidedef.middle_tex = 0;
    auto sidedef_id = room_map.sidedefs.add(room_sidedef);

    math::vec2 corners[4] = {
        {0,    0},
        {0,    2048},
        {2048, 2048},
        {2048, 0}
    };

    std::vector<util::indexed_storage<geometry::linedef>::id_t> line_ids;
    for (int i = 0; i < 4; ++i) {
        geometry::linedef ld;
        ld.v1 = corners[i];
        ld.v2 = corners[(i + 1) % 4];
        ld.front = sidedef_id;
        ld.back = util::indexed_storage<geometry::sidedef>::nullid;

        line_ids.push_back(room_map.linedefs.add(ld));
    }

    geometry::subsector room_subsector;
    room_subsector.lines = line_ids;
    auto subsector_id = room_map.subsectors.add(std::move(room_subsector));

    room_map.root_node_id = geometry::bsp_node::leaf_flag | subsector_id;

    renderer.set_map(&room_map);

    math::vec2 cam_pos(512, 512);
    float cam_height = 128;
    float cam_angle = 0;
    float fov = 90;

	auto start_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = start_time;
	float total_time_ms;
    for(int i = 0; i < 1024; ++i) {
		move_camera(cam_pos, cam_height, cam_angle, fov, i, 1024);
		auto current_time = std::chrono::high_resolution_clock::now();
        float dt_ms = std::chrono::duration<float, std::milli>(current_time - last_frame_time).count();
        total_time_ms = std::chrono::duration<float, std::milli>(current_time - start_time).count();
        last_frame_time = current_time;
		render_frame(cam_pos, cam_height, cam_angle, fov, backend.get(), renderer, r2d, dt_ms, total_time_ms, i);
    }

	int avg_fps = total_time_ms > 0.0f ? static_cast<int>(1024 / (total_time_ms / 1000.0f)) : 0;
	std::cout << "Average FPS: " << avg_fps << std::endl;
    return 0;
}
