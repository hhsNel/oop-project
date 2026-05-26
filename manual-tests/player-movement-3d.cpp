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
#include "assets/asset-manager.h"
#include "entities/player.h"
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
#include "input/evdev/backend.h"

// Expose protected actor fields for this manual test.
template<typename M>
struct inspect : public M {
    using M::M;
    using M::angle;
};

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
    auto sec_id = map.sectors.add(geometry::sector(0.0f, 256.0f, 2, 2, 220));

    // All 4 sidedefs face the same sector; middle_tex uses doom-wall (id 3)
    auto sd0 = map.sidedefs.add(geometry::sidedef(sec_id, 3, 3, 3));
    auto sd1 = map.sidedefs.add(geometry::sidedef(sec_id, 3, 3, 3));
    auto sd2 = map.sidedefs.add(geometry::sidedef(sec_id, 3, 3, 3));
    auto sd3 = map.sidedefs.add(geometry::sidedef(sec_id, 3, 3, 3));

    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    // 4 linedefs – counterclockwise order keeps front side facing inward
    auto id0 = map.linedefs.add(geometry::linedef({0.0f,   0.0f},   {0.0f,   512.0f}, sd0, null_sd));
    auto id1 = map.linedefs.add(geometry::linedef({0.0f,   512.0f}, {512.0f, 512.0f}, sd1, null_sd));
    auto id2 = map.linedefs.add(geometry::linedef({512.0f, 512.0f}, {512.0f, 0.0f},   sd2, null_sd));
    auto id3 = map.linedefs.add(geometry::linedef({512.0f, 0.0f},   {0.0f,   0.0f},   sd3, null_sd));

    // Single subsector (convex room = one BSP leaf)
    auto ss_id = map.subsectors.add(geometry::subsector(
        {id0, id1, id2, id3}, {}));

    // Root is directly the leaf (no internal BSP nodes needed)
    // Use is_leaf/get_id convention: set high bit to mark as leaf
    map.root_node_id = (1u << (sizeof(decltype(map.root_node_id)) * 8 - 1)) | ss_id;

    return map;
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    // ── Rendering backend ────────────────────────────────────────────────────
    auto r_back = std::make_unique<rendering::drm_kms::backend>();
    if (r_back->bad()) { std::cerr << "error: DRM/KMS init failed\n"; return 1; }

    auto modes = (*r_back)("modes"_f);
    if (modes.empty()) { std::cerr << "error: no display modes\n"; return 1; }
    r_back->push_mode(std::move(modes[0]));
    if (r_back->bad()) { std::cerr << "error: set_mode failed\n"; return 1; }

    const int SW = static_cast<int>((*r_back)("width"_f));
    const int SH = static_cast<int>((*r_back)("height"_f));

    // ── Resources ────────────────────────────────────────────────────────────
    util::resource_loader rl;
    auto tex_mgr = assets::asset_manager::load(rl);

    // ── 3D renderer ──────────────────────────────────────────────────────────
    auto room_map = build_room();

    rendering::software_renderer r3d(*r_back, tex_mgr, room_map);

    // ── 2D overlay (info strip) ───────────────────────────────────────────────
    rendering::renderer_2d r2d(*r_back, tex_mgr, tex_mgr.flat_tx_by_id(0));

    // ── Input ────────────────────────────────────────────────────────────────
    auto i_back = std::make_unique<input::evdev::backend>();
    if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return 1; }

    // ── Player ───────────────────────────────────────────────────────────────
    inspect<entities::player> p({256.0f, 256.0f}, 0.0f, 0, 1.0f, 100.0f, 50.0f, 120.0f, 1.0f);
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
            p.move({strafe, fwd});

        // Rotation (per-frame delta, not accumulated total)
        auto mouse   = i_back->get_mouse_state();
        int delta_x  = mouse.x - prev_mouse_x;
        prev_mouse_x = mouse.x;
        if (delta_x != 0)
            p.rotate(static_cast<float>(-delta_x) * MOUSE_SENS);

        p.update(dt);

        // ── RENDER ────────────────────────────────────────────────────────────
        std::memset((*r_back)("mmio"_f), 0,
                    static_cast<std::size_t>((*r_back)("height"_f)) * (*r_back)("pitch"_f));

        // 3D scene
        r3d.render_bsp(static_cast<util::componentized<rendering::sprite>&>(p)("pos"_f), CAM_HEIGHT, p.angle, FOV);

        // Info strip at bottom
        r2d.draw_rect(0, INFO_Y, SW, INFO_H, 0xDD0A0A0A);
        r2d.draw_rect(0, INFO_Y, SW, 1, 0xFF334433);

        float deg = p.angle * (180.0f / std::numbers::pi_v<float>);
        r2d.draw_text("FPS  " + fmt2(fps)
                      + "     POS  x=" + fmt2(static_cast<util::componentized<rendering::sprite>&>(p)("pos"_f)("x"_f)) + "  y=" + fmt2(static_cast<util::componentized<rendering::sprite>&>(p)("pos"_f)("y"_f))
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
