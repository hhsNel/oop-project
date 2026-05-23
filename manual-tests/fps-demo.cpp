// manual-tests/fps-demo.cpp
// Combined manual test: 3D movement + HUD + hitscan combat with a monster.
//
// Room: 512×512, split into 4 subsectors via BSP tree.
// Monster: stands in top-right subsector with visible HP bar.
// Weapons: hitscan pistol, rifle, sniper rifle.
//
// Controls:
//   W / S       – move forward / backward
//   A / D       – strafe left / right
//   Mouse X     – rotate (yaw)
//   LMB         – shoot current weapon
//   R           – reload
//   1-3         – switch weapon
//   F / G / J   – open Damage / Heal / Armor input, then Enter
//   B           – apply Burning effect
//   V           – revive (Game Over only)
//   ESC         – cancel command / exit

#include <iostream>
#include <chrono>
#include <memory>
#include <cmath>
#include <cstring>
#include <numbers>
#include <format>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <algorithm>

#include "util/resource-loader.h"
#include "assets/asset-manager.h"
#include "entities/entities.h"
#include "entities/monsters/monster-basic.h"
#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/katana.h"
#include "combat/weapons/hitscan-firing-mode.h"
#include "combat/weapons/projectile-firing-mode.h"
#include "combat/weapons/plasma-gun.h"
#include "entities/plasma-projectile.h"
#include "entities/slug-projectile.h"
#include "combat/burning.h"
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

// ── Helpers ──────────────────────────────────────────────────────────────────

template<typename M>
struct inspect : public M {
    using M::M;
    float hp()           const { return this->health("current_hp"_f); }
    float max_hp()       const { return this->health("max_hp"_f); }
    float armor()        const { return this->health("armor"_f); }
    float max_armor()    const { return this->health("max_armor"_f); }
    bool  dead()         const { return this->health.is_dead(); }
    int   effect_count() const { return static_cast<int>(this->health("active_effects"_f).size()); }
};

struct weapon_slot {
    std::unique_ptr<combat::weapons::weapon> owned;
    std::string_view name;
    int   reserve_mags;
    float reload_timer;
    float reload_duration;
    bool  is_melee;
    bool  is_auto;
    bool  in_loadout;
};

enum class cmd_mode { none, damage, heal, armor };

static weapon_slot* find_slot(std::vector<weapon_slot>& pool,
                               combat::weapons::weapon* cur)
{
    if (!cur) return nullptr;
    auto it = std::ranges::find_if(pool, [cur](auto& s) { return s.owned.get() == cur; });
    return it != pool.end() ? &*it : nullptr;
}

// ── Build a 4-subsector room ─────────────────────────────────────────────────
//
//  (0,512)───(256,512)───(512,512)
//     │    SS1   │    SS3    │
//     │          │     M     │      M = monster
//  (0,256)───(256,256)───(512,256)
//     │    SS0   │    SS2    │
//     │   P      │           │      P = player start
//  (0,  0)───(256,  0)───(512,  0)
//
// BSP: root splits at x=256 (vertical), children split at y=256 (horizontal)
//
static geometry::map_data build_room() {
    geometry::map_data map;

    // Single sector covering the whole room
    geometry::sector sec;
    sec.floor_height   = 0.0f;
    sec.ceiling_height = 256.0f;
    sec.floor_tex      = 1;
    sec.ceiling_tex    = 1;
    sec.light_level    = 220;
    auto sec_id = map.sectors.add(sec);

    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    // Helper: create a sidedef facing our sector with wall texture 0
    auto make_sd = [&]() {
        geometry::sidedef sd;
        sd.facing_sector = sec_id;
        sd.upper_tex     = 0;
        sd.middle_tex    = 0;
        sd.lower_tex     = 0;
        return map.sidedefs.add(sd);
    };

    // Helper: create a linedef (solid wall — no back sidedef)
    auto make_wall = [&](math::vec2 v1, math::vec2 v2) {
        geometry::linedef ld;
        ld.v1 = v1; ld.v2 = v2;
        ld.front = make_sd();
        ld.back  = null_sd;
        return map.linedefs.add(ld);
    };

    // Helper: create a portal linedef (has both front and back sidedefs)
    auto make_portal = [&](math::vec2 v1, math::vec2 v2) {
        geometry::linedef ld;
        ld.v1 = v1; ld.v2 = v2;
        ld.front = make_sd();
        ld.back  = make_sd();
        return map.linedefs.add(ld);
    };

    // ── Outer walls (CCW: left=UP, top=RIGHT, right=DOWN, bottom=LEFT) ─────
    auto w_left_b  = make_wall({0.0f, 0.0f},      {0.0f, 256.0f});     // SS0 left
    auto w_left_t  = make_wall({0.0f, 256.0f},    {0.0f, 512.0f});     // SS1 left
    auto w_top_l   = make_wall({0.0f, 512.0f},    {256.0f, 512.0f});   // SS1 top
    auto w_top_r   = make_wall({256.0f, 512.0f},  {512.0f, 512.0f});   // SS3 top
    auto w_right_t = make_wall({512.0f, 512.0f},  {512.0f, 256.0f});   // SS3 right
    auto w_right_b = make_wall({512.0f, 256.0f},  {512.0f, 0.0f});     // SS2 right
    auto w_bot_r   = make_wall({512.0f, 0.0f},    {256.0f, 0.0f});     // SS2 bottom
    auto w_bot_l   = make_wall({256.0f, 0.0f},    {0.0f, 0.0f});       // SS0 bottom

    // ── Internal portals ─────────────────────────────────────────────────────
    // Each boundary needs TWO linedefs (opposite directions) for CCW in both subsectors.

    // Vertical x=256, bottom (y=0..256): SS0 right=DOWN, SS2 left=UP
    auto p_vert_bot_down = make_portal({256.0f, 256.0f}, {256.0f, 0.0f});
    auto p_vert_bot_up   = make_portal({256.0f, 0.0f},   {256.0f, 256.0f});

    // Vertical x=256, top (y=256..512): SS1 right=DOWN, SS3 left=UP
    auto p_vert_top_down = make_portal({256.0f, 512.0f}, {256.0f, 256.0f});
    auto p_vert_top_up   = make_portal({256.0f, 256.0f}, {256.0f, 512.0f});

    // Horizontal y=256, left (x=0..256): SS0 top=RIGHT, SS1 bottom=LEFT
    auto p_horiz_l_right = make_portal({0.0f, 256.0f},   {256.0f, 256.0f});
    auto p_horiz_l_left  = make_portal({256.0f, 256.0f}, {0.0f, 256.0f});

    // Horizontal y=256, right (x=256..512): SS2 top=RIGHT, SS3 bottom=LEFT
    auto p_horiz_r_right = make_portal({256.0f, 256.0f}, {512.0f, 256.0f});
    auto p_horiz_r_left  = make_portal({512.0f, 256.0f}, {256.0f, 256.0f});

    // ── Subsectors (CCW: left UP, top RIGHT, right DOWN, bottom LEFT) ────────
    // SS0: bottom-left — (0,0)→(0,256)→(256,256)→(256,0)→(0,0)
    geometry::subsector ss0;
    ss0.lines = {w_left_b, p_horiz_l_right, p_vert_bot_down, w_bot_l};
    auto ss0_id = map.subsectors.add(std::move(ss0));

    // SS1: top-left — (0,256)→(0,512)→(256,512)→(256,256)→(0,256)
    geometry::subsector ss1;
    ss1.lines = {w_left_t, w_top_l, p_vert_top_down, p_horiz_l_left};
    auto ss1_id = map.subsectors.add(std::move(ss1));

    // SS2: bottom-right — (256,0)→(256,256)→(512,256)→(512,0)→(256,0)
    geometry::subsector ss2;
    ss2.lines = {p_vert_bot_up, p_horiz_r_right, w_right_b, w_bot_r};
    auto ss2_id = map.subsectors.add(std::move(ss2));

    // SS3: top-right (monster) — (256,256)→(256,512)→(512,512)→(512,256)→(256,256)
    geometry::subsector ss3;
    ss3.lines = {p_vert_top_up, w_top_r, w_right_t, p_horiz_r_left};
    auto ss3_id = map.subsectors.add(std::move(ss3));

    // ── BSP tree ─────────────────────────────────────────────────────────────
    // Left child: splits SS0/SS1 at y=256
    // cross(pl_dir=(1,0), delta) = delta.y → front when delta.y ≤ 0 (below)
    geometry::bsp_node left_node;
    left_node.pl_coord = {128.0f, 256.0f};
    left_node.pl_dir   = {1.0f, 0.0f};
    left_node.front = geometry::bsp_node::leaf_flag | ss0_id;  // bottom-left (y < 256)
    left_node.back  = geometry::bsp_node::leaf_flag | ss1_id;  // top-left    (y > 256)
    left_node.front_box = {256.0f, 0.0f, 0.0f, 256.0f};
    left_node.back_box  = {512.0f, 256.0f, 0.0f, 256.0f};
    auto left_node_id = map.nodes.add(left_node);

    // Right child: splits SS2/SS3 at y=256
    geometry::bsp_node right_node;
    right_node.pl_coord = {384.0f, 256.0f};
    right_node.pl_dir   = {1.0f, 0.0f};
    right_node.front = geometry::bsp_node::leaf_flag | ss2_id;  // bottom-right (y < 256)
    right_node.back  = geometry::bsp_node::leaf_flag | ss3_id;  // top-right    (y > 256)
    right_node.front_box = {256.0f, 0.0f, 256.0f, 512.0f};
    right_node.back_box  = {512.0f, 256.0f, 256.0f, 512.0f};
    auto right_node_id = map.nodes.add(right_node);

    // Root: splits left/right at x=256
    // cross(pl_dir=(0,1), delta) = -delta.x → front when delta.x ≥ 0 (right)
    geometry::bsp_node root;
    root.pl_coord = {256.0f, 256.0f};
    root.pl_dir   = {0.0f, 1.0f};
    root.front = right_node_id;   // right side (x > 256)
    root.back  = left_node_id;    // left side  (x < 256)
    root.front_box = {512.0f, 0.0f, 256.0f, 512.0f};
    root.back_box  = {512.0f, 0.0f, 0.0f, 256.0f};
    auto root_id = map.nodes.add(root);

    map.root_node_id = root_id;

    return map;
}

// ── Collision helpers ────────────────────────────────────────────────────────

// Closest point on segment (v1→v2) to point p
static math::vec2 closest_on_segment(math::vec2 v1, math::vec2 v2, math::vec2 p) {
    math::vec2 edge = v2 - v1;
    float len2 = edge.sqr_len();
    if (len2 < 0.0001f) return v1;
    float t = std::clamp(math::vec2::dot_product(p - v1, edge) / len2, 0.0f, 1.0f);
    return v1 + edge * t;
}

// Push pos away from solid walls and actors; returns corrected position
static math::vec2 resolve_collisions(math::vec2 pos, float radius,
                                     geometry::map_data const& map,
                                     engine::actor const* monster)
{
    auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

    // Wall collision — push out of any solid linedef
    for (auto e : map.linedefs) {
        geometry::linedef const& ld = e.value;
        if (ld.back != null_sd) continue; // portal, skip

        math::vec2 closest = closest_on_segment(ld.v1, ld.v2, pos);
        math::vec2 diff = pos - closest;
        float dist2 = diff.sqr_len();
        if (dist2 < radius * radius && dist2 > 0.0001f) {
            float dist = std::sqrt(dist2);
            pos = closest + diff * (radius / dist);
        }
    }

    // Monster collision
    if (monster && !monster->is_dead()) {
        constexpr float MONSTER_RADIUS = 32.0f;
        float combined = radius + MONSTER_RADIUS;
        math::vec2 diff = pos - monster->pos;
        float dist2 = diff.sqr_len();
        if (dist2 < combined * combined && dist2 > 0.0001f) {
            float dist = std::sqrt(dist2);
            pos = monster->pos + diff * (combined / dist);
        }
    }

    return pos;
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
    const int half_sw = SW / 2;
    const float half_sh = static_cast<float>(SH) / 2.0f;

    // Cache framebuffer info (avoid repeated componentized lookups per frame)
    auto* const fb_mmio     = (*r_back)("mmio"_f);
    auto  const fb_height   = (*r_back)("height"_f);
    auto  const fb_pitch    = (*r_back)("pitch"_f);
    auto  const fb_clear_sz = static_cast<std::size_t>(fb_height) * fb_pitch;

    // ── Resources ────────────────────────────────────────────────────────────
    util::resource_loader rl;
    auto tex_mgr = assets::asset_manager::load(rl);

    // ── Map ──────────────────────────────────────────────────────────────────
    auto room_map = build_room();

    // ── Monster ──────────────────────────────────────────────────────────────
    inspect<entities::monster_basic> monster;
    monster.pos = {384.0f, 384.0f};   // center of top-right subsector
    monster.z_pos = 0.0f;             // floor level
    monster.tex_id = 0;               // sprite texture 0
    monster.inherent_scale = 0.25f;
    monster.angle = 0.0f;

    // Add monster sprite to subsector 3 (top-right)
    auto monster_ss_id = room_map.get_subsector_id(monster.pos);
    room_map.subsectors[monster_ss_id].sprites.push_back(
        std::unique_ptr<rendering::sprite>(static_cast<rendering::sprite*>(&monster))
    );

    // ── 3D renderer ──────────────────────────────────────────────────────────
    rendering::software_renderer r3d(*r_back, tex_mgr, room_map);

    // ── 2D overlay ───────────────────────────────────────────────────────────
    rendering::renderer_2d r2d(*r_back, tex_mgr, tex_mgr.flat_tx_by_id(0));

    // ── Input ────────────────────────────────────────────────────────────────
    auto i_back = std::make_unique<input::evdev::backend>();
    if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return 1; }

    // ── Player ───────────────────────────────────────────────────────────────
    inspect<entities::player> p(100.0f, 50.0f, 120.0f, 1.0f);
    p.pos = {128.0f, 128.0f};   // center of bottom-left subsector
    p.angle = std::numbers::pi_v<float> / 4.0f;  // 45 degrees, facing toward monster

    constexpr float CAM_HEIGHT  = 120.0f;
    constexpr float FOV         = std::numbers::pi_v<float> / 2.0f;
    constexpr float MOUSE_SENS  = 0.002f;
    constexpr float HALF_PI     = std::numbers::pi_v<float> / 2.0f;

    // Precompute fov_scale (constant for the session)
    const float fov_scale = static_cast<float>(half_sw) / std::tan(FOV / 2.0f);

    // ── Weapon pool ─────────────────────────────────────────────────────────
    std::vector<weapon_slot> pool;
    pool.reserve(7);

    auto add_slot = [&](auto wptr, std::string_view name,
                        int reserve, float reload_dur, bool is_auto = false) {
        pool.push_back({ std::move(wptr), name, reserve, 0.0f, reload_dur,
                         false, is_auto, false });
    };

    // Save raw firing mode pointers before they are moved into weapons,
    // so we can set targets directly (bypassing const componentized access).
    std::vector<combat::weapons::hitscan_firing_mode*> hitscan_modes;
    hitscan_modes.reserve(5);
    std::vector<combat::weapons::projectile_firing_mode*> proj_modes;

    { auto a = std::make_unique<combat::weapons::hitscan_firing_mode>(&room_map, 600.0f, 32.0f);
      hitscan_modes.push_back(a.get());
      add_slot(std::make_unique<combat::weapons::pistol>(std::move(a)),
               "Pistol",  5, 1.5f); }
    { auto a = std::make_unique<combat::weapons::hitscan_firing_mode>(&room_map, 700.0f, 32.0f);
      hitscan_modes.push_back(a.get());
      add_slot(std::make_unique<combat::weapons::rifle>(std::move(a)),
               "Rifle",   4, 2.0f, true); }
    { auto a = std::make_unique<combat::weapons::hitscan_firing_mode>(&room_map, 500.0f, 32.0f);
      hitscan_modes.push_back(a.get());
      add_slot(std::make_unique<combat::weapons::smg>(std::move(a)),
               "SMG",     4, 2.5f, true); }
    { auto a = std::make_unique<combat::weapons::hitscan_firing_mode>(&room_map, 1000.0f, 32.0f);
      hitscan_modes.push_back(a.get());
      add_slot(std::make_unique<combat::weapons::sniper_rifle>(std::move(a)),
               "Sniper",  3, 3.5f); }
    // Katana (melee — short-range hitscan, no ammo)
    { auto a = std::make_unique<combat::weapons::hitscan_firing_mode>(&room_map, 80.0f, 32.0f);
      hitscan_modes.push_back(a.get());
      pool.push_back({ std::make_unique<combat::weapons::katana>(std::move(a)),
                       "Katana", 0, 0.0f, 0.0f, true, false, false }); }

    // Plasma Gun (projectile-based)
    { auto a = std::make_unique<combat::weapons::projectile_firing_mode>(
          &room_map,
          [](math::vec2 pos, float angle, float dmg,
             geometry::map_data* m, std::vector<engine::actor*>* tgts) {
              return std::make_unique<entities::plasma_projectile>(pos, angle, dmg, m, tgts);
          });
      proj_modes.push_back(a.get());
      add_slot(std::make_unique<combat::weapons::plasma_gun>(std::move(a)),
               "Plasma",  3, 2.0f); }

    // Set targets on all firing modes
    std::vector<engine::actor*> target_list{static_cast<engine::actor*>(&monster)};
    for (auto* hm : hitscan_modes)
        (*hm)("targets"_f) = target_list;
    for (auto* pm : proj_modes)
        (*pm)("targets"_f) = target_list;

    // Build player loadout
    for (auto& s : pool) s.in_loadout = true;

    auto rebuild_loadout = [&]() {
        auto* prev = p("current_weapon"_f);
        p("weapons"_f).clear();
        for (auto& s : pool)
            if (s.in_loadout) p("weapons"_f).push_back(s.owned.get());
        if (!p("weapons"_f).empty()) {
            for (int i = 0; i < static_cast<int>(p("weapons"_f).size()); ++i)
                if (p("weapons"_f)[i] == prev) { p.switch_weapons(i); return; }
            p.switch_weapons(0);
        }
    };
    rebuild_loadout();

    // ── HUD layout constants ─────────────────────────────────────────────────
    constexpr int HUD_H = 160;
    const     int HUD_Y = SH - HUD_H;
    const     int COL_W = SW / 3;

    constexpr int PAD  = 14;
    constexpr int VPAD = 10;
    constexpr int CW   = 14;
    constexpr int CH   = 22;
    constexpr int LS   = CH + 4;
    constexpr int SCW  = 11;
    constexpr int SCH  = 16;
    constexpr int SLS  = SCH + 3;
    constexpr int BH   = CH - 4;
    constexpr int BARW = 200;

    constexpr std::uint32_t C_BBKG    = 0xFF333333;
    constexpr std::uint32_t C_HP_HI   = 0xFF00CC44;
    constexpr std::uint32_t C_HP_MID  = 0xFFFF8800;
    constexpr std::uint32_t C_HP_LO   = 0xFFCC2222;
    constexpr std::uint32_t C_ARMOR   = 0xFF3399FF;
    constexpr std::uint32_t C_AMMO    = 0xFFFFDD00;
    constexpr std::uint32_t C_CD      = 0xFFBBBBBB;
    constexpr std::uint32_t C_RELOAD  = 0xFF441111;
    constexpr std::uint32_t C_RED     = 0xFFCC0000;
    constexpr std::uint32_t C_OVERLAY = 0xFF1A1A2A;
    constexpr std::uint32_t TW        = 0xFFFF00;

    // Precomputed crosshair position
    const int cross_cx = half_sw;
    const int cross_cy = HUD_Y / 2;

    auto draw_bar = [&](int x, int y, int w, int h, float cur, float max,
                        std::uint32_t color) {
        r2d.draw_rect(x, y, w, h, C_BBKG);
        if (max > 0.0f) {
            int fill = static_cast<int>(std::clamp(cur / max, 0.0f, 1.0f) * w);
            if (fill > 0) r2d.draw_rect(x, y, fill, h, color);
        }
    };

    // ── Input state ──────────────────────────────────────────────────────────
    cmd_mode    mode = cmd_mode::none;
    std::string num_buf;

    bool monster_removed = false;
    bool prev_lmb   = false;
    bool prev_r     = false;
    bool prev_f     = false;
    bool prev_g     = false;
    bool prev_j     = false;
    bool prev_b     = false;
    bool prev_v     = false;
    bool prev_enter = false;
    bool prev_bs    = false;
    std::array<bool, 10> prev_num{};

    static constexpr std::array<input::key, 10> numkeys = {
        input::key::n0, input::key::n1, input::key::n2, input::key::n3,
        input::key::n4, input::key::n5, input::key::n6, input::key::n7,
        input::key::n8, input::key::n9
    };

    auto edge = [&](input::key k, bool& prev) -> bool {
        bool cur   = i_back->is_key_down(k);
        bool fired = cur && !prev;
        prev = cur;
        return fired;
    };

    // ── Timing ───────────────────────────────────────────────────────────────
    int   prev_mouse_x = 0;
    float fps          = 0.0f;
    float fps_sum      = 0.0f;
    long  fps_frames   = 0;
    auto  last_time    = std::chrono::steady_clock::now();

    float damage_flash_timer = 0.0f;
    constexpr float FLASH_DURATION = 0.45f;

    // ── Game loop ────────────────────────────────────────────────────────────
    while (true) {
        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration<float>(now - last_time).count();
        last_time = now;
        if (dt >= 0.001f) {
            float cur_fps = 1.0f / dt;
            fps = std::lerp(fps, cur_fps, 0.1f);
            fps_sum += cur_fps;
            ++fps_frames;
        }

        i_back->update();
        if (i_back->is_bad()) break;

        bool game_over = p.dead();
        float m_hp_before = monster.hp();
        auto* cur_wpn = p("current_weapon"_f);
        weapon_slot* cur_slot = find_slot(pool, cur_wpn);

        // ESC
        if (i_back->is_key_down(input::key::esc)) {
            if (mode != cmd_mode::none) { mode = cmd_mode::none; num_buf.clear(); }
            else break;
        }

        // Single mouse state query per frame
        auto mouse = i_back->get_mouse_state();

        if (game_over) [[unlikely]] {
            if (edge(input::key::v, prev_v))
                p.heal(p.max_hp());
        } else {
            // ── Movement (WASD) ──────────────────────────────────────────────
            float fwd = 0.0f, strafe = 0.0f;
            if (i_back->is_key_down(input::key::d)) fwd    += dt;
            if (i_back->is_key_down(input::key::a)) fwd    -= dt;
            if (i_back->is_key_down(input::key::s)) strafe += dt;
            if (i_back->is_key_down(input::key::w)) strafe -= dt;
            if (fwd != 0.0f || strafe != 0.0f) {
                p.move({strafe, fwd, 0.0f});
                constexpr float PLAYER_RADIUS = 16.0f;
                p.pos = resolve_collisions(p.pos, PLAYER_RADIUS, room_map, &monster);
            }

            // Rotation
            int delta_x  = mouse.x - prev_mouse_x;
            prev_mouse_x = mouse.x;
            if (delta_x != 0)
                p.rotate(static_cast<float>(-delta_x) * MOUSE_SENS, 0.0f);

            // Weapon switching (1-7)
            if (mode == cmd_mode::none) {
                for (int i = 1; i <= 7 && i <= 9; ++i) {
                    bool down = i_back->is_key_down(numkeys[i]);
                    if (down && !prev_num[i]) {
                        p.switch_weapons(i - 1);
                        cur_wpn = p("current_weapon"_f);
                        cur_slot = find_slot(pool, cur_wpn);
                    }
                    prev_num[i] = down;
                }
            }

            // LMB: shoot via hitscan system
            // Camera forward = (-sin(a), cos(a)), but hitscan uses (cos(θ), sin(θ)),
            // so pass angle + π/2 to align the ray with the crosshair direction.
            {
                bool reloading = cur_slot && cur_slot->reload_timer > 0.0f;
                bool triggered = cur_slot && cur_slot->is_auto
                               ? mouse.left : (mouse.left && !prev_lmb);
                if (!reloading && triggered && cur_wpn)
                {
                    float shoot_angle = p.angle + HALF_PI;
                    cur_wpn->fire(p.pos, shoot_angle);
                }
                prev_lmb = mouse.left;
            }

            // R: reload
            {
                bool r_down = i_back->is_key_down(input::key::r);
                if (r_down && !prev_r && cur_slot
                    && !cur_slot->is_melee
                    && cur_slot->reserve_mags > 0
                    && cur_slot->reload_timer <= 0.0f)
                {
                    --cur_slot->reserve_mags;
                    cur_slot->reload_timer = cur_slot->reload_duration;
                }
                prev_r = r_down;
            }

            // F / G / J: Damage / Heal / Armor commands (remapped from D/H/A)
            if (mode == cmd_mode::none) {
                if (edge(input::key::f, prev_f)) { mode = cmd_mode::damage; num_buf.clear(); }
                if (edge(input::key::g, prev_g)) { mode = cmd_mode::heal;   num_buf.clear(); }
                if (edge(input::key::j, prev_j)) { mode = cmd_mode::armor;  num_buf.clear(); }
                if (edge(input::key::b, prev_b))
                    p.add_effect(std::make_unique<combat::burning>(5.0f, 5));
            }

            // Number entry for commands
            if (mode != cmd_mode::none) [[unlikely]] {
                for (int i = 0; i < 10; ++i) {
                    bool down = i_back->is_key_down(numkeys[i]);
                    if (down && !prev_num[i]) num_buf += static_cast<char>('0' + i);
                    prev_num[i] = down;
                }
                if (edge(input::key::backspace, prev_bs) && !num_buf.empty())
                    num_buf.pop_back();
                if (edge(input::key::enter, prev_enter)) {
                    if (!num_buf.empty()) {
                        float val = static_cast<float>(std::stoi(num_buf));
                        if      (mode == cmd_mode::damage) { p.take_damage(val); damage_flash_timer = FLASH_DURATION; }
                        else if (mode == cmd_mode::heal)   p.heal(val);
                        else if (mode == cmd_mode::armor)  p.add_shield(val);
                    }
                    mode = cmd_mode::none;
                    num_buf.clear();
                }
            }

            // Tick reload timers
            for (auto& s : pool) {
                if (s.reload_timer > 0.0f) {
                    s.reload_timer -= dt;
                    if (s.reload_timer <= 0.0f) {
                        s.reload_timer = 0.0f;
                        s.owned->reload();
                    }
                }
            }
        }

        float hp_before = p.hp();
        p.update(dt);
        if (p.hp() < hp_before)
            damage_flash_timer = FLASH_DURATION;

        for (auto* pm : proj_modes)
            pm->update(dt);

        if (!monster.dead())
            monster.update(dt);

        // Remove monster sprite from map once when killed
        if (monster.dead() && !monster_removed) {
            auto ss_id = room_map.get_subsector_id(monster.pos);
            auto& sprites = room_map.subsectors[ss_id].sprites;
            auto it = std::find_if(sprites.begin(), sprites.end(),
                [&](std::unique_ptr<rendering::sprite> const& s) {
                    return s.get() == static_cast<rendering::sprite*>(&monster);
                });
            if (it != sprites.end()) {
                it->release();
                sprites.erase(it);
            }
            monster_removed = true;
        }

        // Update all weapons (ensures projectiles keep moving even after weapon switch)
        for (auto& s : pool)
            if (s.owned) s.owned->update(dt);

        // Monster hit flash (checked after all damage sources: hitscan + projectiles)
        if (monster.hp() < m_hp_before)
            monster.hit_flash = 1.0f;
        if (monster.hit_flash > 0.0f)
            monster.hit_flash = std::max(0.0f, monster.hit_flash - dt * 5.0f);

        // Refresh cached weapon pointer after update
        cur_wpn  = p("current_weapon"_f);
        cur_slot = find_slot(pool, cur_wpn);
        if (damage_flash_timer > 0.0f)
            damage_flash_timer -= dt;

        // ── RENDER ───────────────────────────────────────────────────────────
        std::memset(fb_mmio, 0, fb_clear_sz);

        // Precompute trig for this frame (reused by render_bsp and monster HP bar)
        float cos_a = std::cos(p.angle);
        float sin_a = std::sin(p.angle);

        // 3D scene
        r3d.render_bsp(p.pos, CAM_HEIGHT, p.angle, FOV);

        // Red crosshair at center of 3D viewport
        r2d.draw_rect(cross_cx - 8, cross_cy,     16, 2, 0xFFCC0000);
        r2d.draw_rect(cross_cx,     cross_cy - 8, 2, 16, 0xFFCC0000);

        // Damage flash overlay
        if (damage_flash_timer > 0.0f) {
            float t     = std::clamp(damage_flash_timer / FLASH_DURATION, 0.0f, 1.0f);
            auto  alpha = static_cast<std::uint32_t>(t * 160);
            r2d.draw_rect(0, 0, SW, HUD_Y, (alpha << 24) | 0x00CC0000);
        }

        // ── Monster HP bar (projected to screen) ─────────────────────────────
        if (!monster.dead()) {
            math::vec2 rel = monster.pos - p.pos;
            float vx =  rel("x"_f) * cos_a + rel("y"_f) * sin_a;
            float vy = -rel("x"_f) * sin_a + rel("y"_f) * cos_a;

            if (vy > 1.0f) {
                float scale = fov_scale / vy;
                int screen_x = half_sw + static_cast<int>(vx / vy * fov_scale);

                assets::texture const& spr_tex = tex_mgr.sprite_tx_by_id(monster.tex_id);
                float scr_y_bot = half_sh - (monster.z_pos - CAM_HEIGHT) * scale;
                float scr_y_top = scr_y_bot - spr_tex.height * scale * monster.inherent_scale;

                int bar_y = static_cast<int>(scr_y_top) - 6;

                int bar_w = static_cast<int>(80.0f * scale * monster.inherent_scale);
                int bar_h = static_cast<int>(8.0f * scale * monster.inherent_scale);
                bar_w = std::clamp(bar_w, 20, 300);
                bar_h = std::clamp(bar_h, 4, 16);

                int bar_x = screen_x - bar_w / 2;

                if (bar_y > 0 && bar_y < HUD_Y && bar_x + bar_w > 0 && bar_x < SW) {
                    float m_hp     = monster.hp();
                    float m_max_hp = monster.max_hp();
                    float ratio    = m_max_hp > 0.0f ? m_hp / m_max_hp : 0.0f;

                    r2d.draw_rect(bar_x, bar_y, bar_w, bar_h, 0xCC000000);
                    int fill_w = static_cast<int>(std::clamp(ratio, 0.0f, 1.0f) * bar_w);
                    if (fill_w > 0) {
                        auto color = ratio > 0.6f ? C_HP_HI : ratio > 0.3f ? C_HP_MID : C_HP_LO;
                        r2d.draw_rect(bar_x, bar_y, fill_w, bar_h, color);
                    }
                    r2d.draw_text(std::format("{:.1f}/{:.1f}", m_hp, m_max_hp),
                                  bar_x, bar_y - 14, 8, 12, 0xFFFFFF);
                }
            }
        }

        // ── HUD ──────────────────────────────────────────────────────────────
        r2d.draw_texture(tex_mgr.flat_tx_by_id(1), 0, HUD_Y, SW, HUD_H);
        r2d.draw_rect(0,           HUD_Y, COL_W,            HUD_H, 0xBB080810);
        r2d.draw_rect(COL_W,       HUD_Y, COL_W,            HUD_H, 0xBB081008);
        r2d.draw_rect(COL_W * 2,   HUD_Y, SW - COL_W * 2,  HUD_H, 0xBB100808);

        // COL 1: Player status
        {
            int cx = PAD;
            int cy = HUD_Y + VPAD;

            float p_hp  = p.hp(),  p_mhp = p.max_hp();
            float p_ar  = p.armor(), p_mar = p.max_armor();

            float hp_ratio = p_mhp > 0.0f ? p_hp / p_mhp : 0.0f;
            auto  hp_color = hp_ratio > 0.6f ? C_HP_HI
                           : hp_ratio > 0.3f ? C_HP_MID : C_HP_LO;
            r2d.draw_text("HP:", cx, cy + 2, CW, CH - 4, TW);
            draw_bar(cx + 40, cy, BARW, BH, p_hp, p_mhp, hp_color);
            r2d.draw_text(std::format("{:.1f}/{:.1f}", p_hp, p_mhp),
                          cx + 44 + BARW, cy + 2, CW - 2, CH - 4, TW);
            cy += LS;

            r2d.draw_text("AR:", cx, cy + 2, CW, CH - 4, TW);
            draw_bar(cx + 40, cy, BARW, BH, p_ar, p_mar, C_ARMOR);
            r2d.draw_text(std::format("{:.1f}/{:.1f}", p_ar, p_mar),
                          cx + 44 + BARW, cy + 2, CW - 2, CH - 4, TW);
            cy += LS;

            if (p.effect_count() > 0) {
                r2d.draw_rect(cx, cy, 180, CH, 0xFF331100);
                r2d.draw_text("FX: BURNING", cx + 4, cy + 2, CW, CH - 4, TW);
            } else {
                r2d.draw_text("FX: none", cx, cy + 2, CW, CH - 4, TW);
            }
            cy += LS + 8;

            // FPS counter (below player status)
            r2d.draw_text(std::format("FPS: {:.2f}", fps), cx, cy, SCW, SCH, TW);
            cy += SLS;
            if (fps_frames > 0)
                r2d.draw_text(std::format("AVG: {:.2f}", fps_sum / static_cast<float>(fps_frames)),
                              cx, cy, SCW, SCH, TW);
        }

        // COL 2: Weapon info
        {
            int cx = COL_W + PAD;
            int cy = HUD_Y + VPAD;

            if (!cur_wpn || !cur_slot) {
                r2d.draw_text("Weapon: (none)", cx, cy, CW, CH, TW);
            } else {
                auto& wpn_ref = *cur_wpn;
                int wpn_idx = p("current_weapon_index"_f);
                int wpn_cnt = static_cast<int>(p("weapons"_f).size());
                int ammo_cur = wpn_ref("ammo_count"_f);
                int ammo_max = wpn_ref("max_ammo"_f);

                r2d.draw_text(std::format("{}{} {}/{}",
                    cur_slot->name,
                    cur_slot->is_auto ? " [AUTO]" : "",
                    wpn_idx + 1, wpn_cnt),
                    cx, cy, CW, CH, TW);
                cy += LS;

                if (cur_slot->reload_timer > 0.0f) {
                    r2d.draw_text(std::format("AMMO: {} / {}", ammo_cur, ammo_max),
                        cx, cy, CW, CH, TW);
                    cy += LS;
                    r2d.draw_rect(cx - PAD, cy - 2, COL_W, CH + 6, C_RELOAD);
                    r2d.draw_text(std::format("RELOADING  {:.1f}s", cur_slot->reload_timer),
                                  cx + 4, cy, CW, CH, TW);
                    draw_bar(cx + 230, cy + 2, 150, BH - 2,
                             cur_slot->reload_duration - cur_slot->reload_timer,
                             cur_slot->reload_duration, C_CD);
                } else {
                    r2d.draw_text("AMMO:", cx, cy + 2, CW, CH - 4, TW);
                    draw_bar(cx + 65, cy, BARW, BH,
                             static_cast<float>(ammo_cur),
                             static_cast<float>(ammo_max), C_AMMO);
                    r2d.draw_text(std::format("{}/{}", ammo_cur, ammo_max),
                                  cx + 69 + BARW, cy + 2, CW - 2, CH - 4, TW);
                    cy += LS;

                    r2d.draw_text(std::format("MAGS: {}", cur_slot->reserve_mags),
                                  cx, cy, CW, CH, TW);
                    cy += LS;

                    if (!cur_slot->is_auto) {
                        float lst = wpn_ref("last_shot_time"_f);
                        float fr  = wpn_ref("fire_rate"_f);
                        float mcd = fr > 0.0f ? 1.0f / fr : 1.0f;
                        r2d.draw_text("SHOT CD:", cx, cy + 2, CW, CH - 4, TW);
                        draw_bar(cx + 100, cy, BARW, BH,
                                 mcd - std::min(lst, mcd), mcd, C_CD);
                    }
                }
            }
        }

        // COL 3: Loadout + FPS
        {
            int cx = COL_W * 2 + PAD;
            int cy = HUD_Y + VPAD;

            r2d.draw_text("LOADOUT:", cx, cy, CW, CH, TW);
            cy += LS;

            auto& weapons = p("weapons"_f);
            int wpn_idx = p("current_weapon_index"_f);
            int wpn_size = static_cast<int>(weapons.size());
            for (int i = 0; i < wpn_size; ++i) {
                bool active = (i == wpn_idx);
                std::string_view slot_name;
                for (auto& s : pool)
                    if (s.owned.get() == weapons[i])
                        { slot_name = s.name; break; }

                r2d.draw_text(std::format("{}: {}{}{}", i + 1,
                    active ? "[" : " ", slot_name, active ? "]" : ""),
                    cx, cy, SCW, SCH, TW);
                cy += SLS;
            }
        }

        // ── Game Over overlay ────────────────────────────────────────────────
        if (game_over) [[unlikely]] {
            constexpr int W = 520, H = 116;
            const int X = half_sw - W / 2, Y = SH / 2 - H - 80;
            r2d.draw_rect(X, Y, W, H, C_RED);
            r2d.draw_text("GAME  OVER", X + 24, Y + 10, 36, 60, 0u);
            r2d.draw_text("[V] Revive              [ESC] Exit",
                          X + 24, Y + 82, CW, CH, 0u);
        }

        // ── Command input overlay ────────────────────────────────────────────
        if (mode != cmd_mode::none) [[unlikely]] {
            const char* prompt = mode == cmd_mode::damage ? "Damage amount"
                               : mode == cmd_mode::heal   ? "Heal amount"
                               :                            "Armor amount";
            constexpr int W = 540, H = 86;
            const int X = half_sw - W / 2, Y = SH / 2 - H - 80;
            r2d.draw_rect(X, Y, W, H, C_OVERLAY);
            r2d.draw_rect(X, Y, W, 2, 0xFF5555AA);
            r2d.draw_text(std::format("> {}: {}_", prompt, num_buf),
                          X + 14, Y + 14, CW, CH, TW);
            r2d.draw_text("[0-9] type   [Bsp] delete   [Enter] apply   [Esc] cancel",
                          X + 14, Y + 14 + LS, CW - 2, CH - 4, TW);
        }

        // ── Monster status text ──────────────────────────────────────────────
        if (!monster.dead()) {
            r2d.draw_text(std::format("MONSTER HP: {:.1f}/{:.1f}", monster.hp(), monster.max_hp()),
                          PAD, 10, SCW, SCH, 0xFFFF00);
        } else {
            r2d.draw_text("MONSTER DEAD", PAD, 10, SCW, SCH, C_RED);
        }

        r_back->flush();
        r_back->wait_for_vsync();
    }

    // Remove monster sprite from subsector before destruction
    // (it's not heap-allocated, so we must release the unique_ptr without deleting)
    for (auto ss_entry : room_map.subsectors) {
        auto& sprites = ss_entry.value.sprites;
        for (auto it = sprites.begin(); it != sprites.end(); ++it) {
            if (it->get() == static_cast<rendering::sprite*>(&monster)) {
                it->release();
                sprites.erase(it);
                break;
            }
        }
    }

    if (fps_frames > 0)
        std::cout << std::format("Average FPS: {:.2f}  ({} frames)\n",
                                 fps_sum / static_cast<float>(fps_frames), fps_frames);

    return 0;
}
