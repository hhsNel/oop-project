// manual-tests/player-movement-3d.cpp
// Manual test: 3D movement in a single 4-walled room.
//
// Wall texture  : top half   of "Doom wall sprites.png"  (doom-wall.btx,  wall id 3)
// Floor texture : bottom half of "Doom wall sprites.png"  (doom-floor.btx, flat id 2)
//
// Controls:
//   W / S     – move forward / backward
//   A / D     – strafe left / right
//   Mouse X   – rotate (yaw)
//   ESC       – exit

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <cmath>
#include <cstring>
#include <numbers>
#include <iomanip>
#include <sstream>

#include "util/resource-loader.h"
#include "graphics/texture-manager.h"
#include "entities/entities.h"
#include "geometry/map-data.h"
#include "geometry/sector.h"
#include "geometry/sidedef.h"
#include "geometry/linedef.h"
#include "geometry/subsector.h"
#include "geometry/bsp-node.h"
#include "rendering/software-renderer.h"
#include "rendering/renderer-2d.h"
#include "rendering/drm-kms/backend.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"

static std::string fmt2(float v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

// ── Build a simple square room ────────────────────────────────────────────────
//
//  (0,512)──────(512,512)
//     │               │
//     │   player→     │
//     │               │
//  (0,  0)──────(512,  0)
//
static geometry::map_data build_room() {
    geometry::map_data map;

    // Sector: floor=0, ceiling=256, doom-floor texture, full brightness
    geometry::sector sec;
    sec.floor_height   = 0.0f;
    sec.ceiling_height = 256.0f;
    sec.floor_tex      = 2;   // doom-floor.btx  (flat id 2)
    sec.ceiling_tex    = 2;   // same for ceiling
    sec.light_level    = 220;
    auto sec_id = map.sectors.add(sec);

    // All 4 sidedefs face the same sector; middle_tex uses doom-wall
    geometry::sidedef sd;
    sd.facing_sector = sec_id;
    sd.upper_tex     = 3;   // doom-wall.btx (wall id 3)
    sd.middle_tex    = 3;
    sd.lower_tex     = 3;
    auto sd0 = map.sidedefs.add(sd);
    auto sd1 = map.sidedefs.add(sd);
    auto sd2 = map.sidedefs.add(sd);
    auto sd3 = map.sidedefs.add(sd);

    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    // 4 linedefs – counterclockwise order keeps front side facing inward
    geometry::linedef ld;
    ld.back = null_sd;

    ld.v1 = {0.0f,   0.0f};   ld.v2 = {0.0f,   512.0f}; ld.front = sd0;
    auto id0 = map.linedefs.add(ld);

    ld.v1 = {0.0f,   512.0f}; ld.v2 = {512.0f, 512.0f}; ld.front = sd1;
    auto id1 = map.linedefs.add(ld);

    ld.v1 = {512.0f, 512.0f}; ld.v2 = {512.0f, 0.0f};   ld.front = sd2;
    auto id2 = map.linedefs.add(ld);

    ld.v1 = {512.0f, 0.0f};   ld.v2 = {0.0f,   0.0f};   ld.front = sd3;
    auto id3 = map.linedefs.add(ld);

    // Single subsector (convex room = one BSP leaf)
    geometry::subsector ss;
    ss.lines = {id0, id1, id2, id3};
    auto ss_id = map.subsectors.add(std::move(ss));

    // Root is directly the leaf (no internal BSP nodes needed)
    map.root_node_id = geometry::bsp_node::leaf_flag | ss_id;

    return map;
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    // ── Rendering backend ────────────────────────────────────────────────────
    auto r_back = std::make_unique<rendering::drm_kms::backend>();
    if (r_back->is_bad()) { std::cerr << "error: DRM/KMS init failed\n"; return 1; }

    auto modes = r_back->get_modes();
    if (modes.empty()) { std::cerr << "error: no display modes\n"; return 1; }
    r_back->set_mode(std::move(modes[0]));
    if (r_back->is_bad()) { std::cerr << "error: set_mode failed\n"; return 1; }

    const int SW = static_cast<int>(r_back->get_width());
    const int SH = static_cast<int>(r_back->get_height());

    // ── Resources ────────────────────────────────────────────────────────────
    util::resource_loader rl;
    auto tex_mgr = graphics::texture_manager::load(rl);

    // ── 3D renderer ──────────────────────────────────────────────────────────
    auto room_map = build_room();

    rendering::software_renderer r3d;
    r3d.set_target(r_back.get());
    r3d.set_texture_manager(&tex_mgr);
    r3d.set_map(&room_map);

    // ── 2D overlay (info strip) ───────────────────────────────────────────────
    rendering::renderer_2d r2d;
    r2d.set_target(r_back.get());
    r2d.set_texture_manager(&tex_mgr);
    r2d.set_font_texture(&tex_mgr.flat_tx_by_id(0));  // font atlas

    // ── Input ────────────────────────────────────────────────────────────────
    auto i_back = std::make_unique<input::evdev::backend>();
    if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return 1; }

    // ── Player ───────────────────────────────────────────────────────────────
    entities::player p(100.0f, 50.0f, 120.0f, 1.0f);
    p.pos = {256.0f, 256.0f};   // center of room
    p.angle    = 0.0f;               // facing +X

    const float CAM_HEIGHT  = 120.0f;   // eye height (floor=0, ceil=256)
    const float FOV         = std::numbers::pi_v<float> / 2.0f;  // 90°
    const float MOUSE_SENS  = 0.002f;

    int   prev_mouse_x = 0;
    float fps          = 0.0f;
    float fps_sum      = 0.0f;
    long  fps_frames   = 0;
    auto  last_time    = std::chrono::steady_clock::now();

    // ── Info strip layout ────────────────────────────────────────────────────
    const int INFO_H = 52;
    const int INFO_Y = SH - INFO_H;
    const std::uint32_t TW = 0xffffff;

    // ── Game loop ─────────────────────────────────────────────────────────────
    while (true) {
        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration<float>(now - last_time).count();
        last_time = now;
        if (dt >= 0.001f) {
            float cur = 1.0f / dt;
            fps = fps * 0.9f + cur * 0.1f;
            fps_sum += cur;
            ++fps_frames;
        }

        i_back->update();
        if (i_back->is_bad()) break;
        if (i_back->is_key_down(input::key::esc)) break;

        // Movement
        float fwd = 0.0f, strafe = 0.0f;
        if (i_back->is_key_down(input::key::d)) fwd    += dt;
        if (i_back->is_key_down(input::key::a)) fwd    -= dt;
        if (i_back->is_key_down(input::key::s)) strafe += dt;
        if (i_back->is_key_down(input::key::w)) strafe -= dt;
        if (fwd != 0.0f || strafe != 0.0f)
            p.move({strafe, fwd, 0.0f});

        // Rotation (per-frame delta, not accumulated total)
        auto mouse   = i_back->get_mouse_state();
        int delta_x  = mouse.x - prev_mouse_x;
        prev_mouse_x = mouse.x;
        if (delta_x != 0)
            p.rotate(static_cast<float>(-delta_x) * MOUSE_SENS, 0.0f);

        p.update(dt);

        // ── RENDER ────────────────────────────────────────────────────────────
        std::memset(r_back->get_mmio(), 0,
                    static_cast<std::size_t>(r_back->get_height()) * r_back->get_pitch());

        // 3D scene
        r3d.render_bsp(p.pos, CAM_HEIGHT, p.angle, FOV);

        // Info strip at bottom
        r2d.draw_rect(0, INFO_Y, SW, INFO_H, 0xDD0A0A0A);
        r2d.draw_rect(0, INFO_Y, SW, 1, 0xFF334433);

        float deg = p.angle * (180.0f / std::numbers::pi_v<float>);
        r2d.draw_text("FPS  " + fmt2(fps)
                      + "     POS  x=" + fmt2(p.pos("x"_f)) + "  y=" + fmt2(p.pos("y"_f))
                      + "     ANG  " + fmt2(p.angle) + " rad  (" + fmt2(deg) + " deg)",
                      14, INFO_Y + 8, 13, 20, TW);
        r2d.draw_text("W/S: forward/back     A/D: strafe     Mouse: rotate     ESC: exit",
                      14, INFO_Y + 30, 10, 15, TW);

        r_back->flush();
        r_back->wait_for_vsync();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    if (fps_frames > 0)
        std::cout << "Average FPS: " << fmt2(fps_sum / static_cast<float>(fps_frames))
                  << "  (" << fps_frames << " frames)\n";

    return 0;
}
