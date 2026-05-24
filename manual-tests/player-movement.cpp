// manual-tests/player-movement.cpp
// Manual test: player::move() and player::rotate()
//
// Displays a top-down view of the player moving in 2D space.
//
// Controls:
//   W / S       – move forward / backward
//   A / D       – strafe left / right
//   Mouse X     – rotate (yaw)
//   ESC         – exit

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <cmath>
#include <cstring>
#include <numbers>
#include <iomanip>
#include <sstream>

#include "entities/player.h"
#include "util/resource-loader.h"
#include "assets/asset-manager.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"
#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"

static std::string fmt2(float v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

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

    rendering::renderer_2d r2d(*r_back, tex_mgr, tex_mgr.flat_tx_by_id(0));

    // ── Input ────────────────────────────────────────────────────────────────
    auto i_back = std::make_unique<input::evdev::backend>();
    if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return 1; }

    // ── Player ───────────────────────────────────────────────────────────────
    entities::player p({0.0f, 0.0f}, 0.0f, 0, 1.0f, 100.0f, 50.0f, 4.0f, 1.0f);
    p.angle    = 0.0f;

    // Helper to access pos through the correct componentized base
    auto ppos = [&]() -> auto& {
        return static_cast<util::componentized<rendering::sprite>&>(p)("pos"_f);
    };

    // ── Layout ───────────────────────────────────────────────────────────────
    // Top-down view fills most of the screen; info strip at the bottom.
    const int INFO_H  = 80;
    const int VIEW_H  = SH - INFO_H;
    const int CX      = SW / 2;   // screen centre (player is always here)
    const int CY      = VIEW_H / 2;
    const float SCALE = 48.0f;    // pixels per world unit

    // Colors
    const std::uint32_t C_BG       = 0xFF0D0D0D;
    const std::uint32_t C_GRID     = 0xFF222222;
    const std::uint32_t C_AXIS     = 0xFF333366;
    const std::uint32_t C_PLAYER   = 0xFF22DD55;
    const std::uint32_t C_DIR      = 0xFFFFFFFF;
    const std::uint32_t C_INFO_BG  = 0xFF111111;
    const std::uint32_t TW         = 1u;

    // Draws a line by painting 2×2 dots along it (no line primitive in renderer)
    auto draw_line = [&](float x0, float y0, float x1, float y1,
                         std::uint32_t color, int dot = 2) {
        float dx = x1 - x0, dy = y1 - y0;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 0.5f) return;
        float step = static_cast<float>(dot);
        int   n    = static_cast<int>(len / step);
        for (int i = 0; i <= n; ++i) {
            float t  = (i * step) / len;
            int   px = static_cast<int>(x0 + dx * t);
            int   py = static_cast<int>(y0 + dy * t);
            r2d.draw_rect(px, py, dot, dot, color);
        }
    };

    const float MOUSE_SENS = 0.004f;  // radians per raw mouse pixel
    int prev_mouse_x = 0;

    auto last_time = std::chrono::steady_clock::now();

    // ── Game loop ─────────────────────────────────────────────────────────────
    while (true) {
        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        i_back->update();
        if (i_back->is_bad()) break;
        if (i_back->is_key_down(input::key::esc)) break;

        // ── Input → player ───────────────────────────────────────────────────
        float fwd    = 0.0f, strafe = 0.0f;
        if (i_back->is_key_down(input::key::w)) fwd    += dt;
        if (i_back->is_key_down(input::key::s)) fwd    -= dt;
        if (i_back->is_key_down(input::key::a)) strafe += dt;
        if (i_back->is_key_down(input::key::d)) strafe -= dt;

        if (fwd != 0.0f || strafe != 0.0f)
            p.move({strafe, fwd});

        auto mouse   = i_back->get_mouse_state();
        int delta_x  = mouse.x - prev_mouse_x;
        prev_mouse_x = mouse.x;
        if (delta_x != 0)
            p.rotate(static_cast<float>(delta_x) * MOUSE_SENS);

        p.update(dt);

        // ── World → screen projection ─────────────────────────────────────────
        // Player is always at (CX, CY); everything else is offset from there.
        auto world_to_screen = [&](float wx, float wy) -> std::pair<int, int> {
            return {
                static_cast<int>(CX + (wx - ppos()("x"_f)) * SCALE),
                static_cast<int>(CY + (wy - ppos()("y"_f)) * SCALE)
            };
        };

        // ── RENDER ────────────────────────────────────────────────────────────
        std::memset((*r_back)("mmio"_f), 0,
                    static_cast<std::size_t>((*r_back)("height"_f)) * (*r_back)("pitch"_f));

        // Background
        r2d.draw_rect(0, 0, SW, VIEW_H, C_BG);

        // World grid — lines every 1 unit, thicker every 5 units
        {
            float visible_units_x = (SW / 2.0f) / SCALE + 1.0f;
            float visible_units_y = (VIEW_H / 2.0f) / SCALE + 1.0f;
            int grid_x0 = static_cast<int>(std::floor(ppos()("x"_f) - visible_units_x));
            int grid_x1 = static_cast<int>(std::ceil (ppos()("x"_f) + visible_units_x));
            int grid_y0 = static_cast<int>(std::floor(ppos()("y"_f) - visible_units_y));
            int grid_y1 = static_cast<int>(std::ceil (ppos()("y"_f) + visible_units_y));

            for (int gx = grid_x0; gx <= grid_x1; ++gx) {
                int sx     = static_cast<int>(CX + (gx - ppos()("x"_f)) * SCALE);
                int sy_top = static_cast<int>(CY + (grid_y0 - ppos()("y"_f)) * SCALE);
                int sy_bot = static_cast<int>(CY + (grid_y1 - ppos()("y"_f)) * SCALE);
                std::uint32_t col = (gx % 5 == 0) ? C_AXIS : C_GRID;
                draw_line(static_cast<float>(sx), static_cast<float>(sy_top),
                          static_cast<float>(sx), static_cast<float>(sy_bot), col);
            }
            for (int gy = grid_y0; gy <= grid_y1; ++gy) {
                int sx_l = static_cast<int>(CX + (grid_x0 - ppos()("x"_f)) * SCALE);
                int sx_r = static_cast<int>(CX + (grid_x1 - ppos()("x"_f)) * SCALE);
                int sy   = static_cast<int>(CY + (gy    - ppos()("y"_f)) * SCALE);
                std::uint32_t col = (gy % 5 == 0) ? C_AXIS : C_GRID;
                draw_line(static_cast<float>(sx_l), static_cast<float>(sy),
                          static_cast<float>(sx_r), static_cast<float>(sy), col);
            }
        }

        // World origin marker (bright cross at 0,0)
        {
            auto [ox, oy] = world_to_screen(0.0f, 0.0f);
            r2d.draw_rect(ox - 4, oy - 1, 9, 3, 0xFF446688);
            r2d.draw_rect(ox - 1, oy - 4, 3, 9, 0xFF446688);
        }

        // Player direction ray
        {
            float ray_len = 2.5f * SCALE;  // 2.5 world units long
            float ex = CX + std::cos(p.angle) * ray_len;
            float ey = CY + std::sin(p.angle) * ray_len;
            draw_line(static_cast<float>(CX), static_cast<float>(CY), ex, ey, C_DIR, 2);
        }

        // Player dot (filled 10×10)
        r2d.draw_rect(CX - 5, CY - 5, 10, 10, C_PLAYER);

        // ── Info strip ────────────────────────────────────────────────────────
        r2d.draw_rect(0, VIEW_H, SW, INFO_H, C_INFO_BG);
        r2d.draw_rect(0, VIEW_H, SW, 1, 0xFF334433);

        float deg = p.angle * (180.0f / std::numbers::pi_v<float>);
        r2d.draw_text("POS:  x=" + fmt2(ppos()("x"_f)) + "  y=" + fmt2(ppos()("y"_f)),
                      16, VIEW_H + 10, 13, 20, TW);
        r2d.draw_text("ANG:  " + fmt2(p.angle) + " rad  (" + fmt2(deg) + " deg)",
                      16, VIEW_H + 34, 13, 20, TW);
        r2d.draw_text("W/S:forward/back   A/D:strafe   Mouse:rotate   ESC:exit",
                      16, VIEW_H + 58, 10, 15, TW);

        r_back->flush();
        r_back->wait_for_vsync();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
