#include <iostream>
#include <string>
#include <memory>
#include <numbers>

#include "combat/weapons/hitscan-firing-mode.h"
#include "combat/weapons/pistol.h"
#include "entities/monsters/monster-basic.h"
#include "geometry/map-data.h"
#include "geometry/linedef.h"
#include "geometry/sidedef.h"
#include "engine/world.h"

// Exposes health fields for test inspection via componentized.
template<typename M>
struct inspect : public M {
    using M::M;
    float hp()   const { return this->health("current_hp"_f); }
    float armor() const { return this->health("armor"_f); }
};

static constexpr math::vec2 ORIGIN{0.0f, 0.0f};

static void result(float v) {
    std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(const std::string& s) {
    std::cout << "RESULT " << s << std::endl;
}

// Build a solid wall linedef (back = nullid).
static geometry::linedef solid_wall(math::vec2 a, math::vec2 b) {
    return geometry::linedef(a, b, 1, util::indexed_storage<geometry::sidedef>::nullid);
}

// Build a portal linedef (both sides set — passes through).
static geometry::linedef portal_wall(math::vec2 a, math::vec2 b) {
    return geometry::linedef(a, b, 1, 2);
}

int main() {
    geometry::map_data empty_md;

    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        // ── hs_direct_hit ────────────────────────────────────────────────
        // Actor straight ahead — must be hit.
        // Camera convention: angle=0 looks +Y. Actor at (0,100), shoot angle=0, dmg=10
        // Expected: HP = 20
        if (cmd == "hs_direct_hit") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_miss_side ─────────────────────────────────────────────────
        // Actor perpendicular to ray — must miss.
        // Shoot angle=0 (+Y), actor at (100, 0) — off to the side.
        // Expected: HP = 30
        } else if (cmd == "hs_miss_side") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{100.0f, 0.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_miss_behind ───────────────────────────────────────────────
        // Actor behind the shooter — must miss (t < 0).
        // Shoot angle=0 (+Y), actor at (0, -100) — behind.
        // Expected: HP = 30
        } else if (cmd == "hs_miss_behind") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, -100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_miss_range ────────────────────────────────────────────────
        // Actor beyond max_range — must miss.
        // Actor at (0, 600), range = 500. Shoot angle=0 (+Y).
        // Expected: HP = 30
        } else if (cmd == "hs_miss_range") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 600.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w, 500.0f);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_hit_in_range ──────────────────────────────────────────────
        // Actor within max_range — must be hit.
        // Actor at (0, 400), range = 500. Shoot angle=0 (+Y).
        // Expected: HP = 20
        } else if (cmd == "hs_hit_in_range") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 400.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w, 500.0f);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_wall_blocks ───────────────────────────────────────────────
        // Solid wall between shooter and actor — actor must NOT be hit.
        // Wall at y=50, actor at (0, 100). Shoot angle=0 (+Y).
        // Expected: HP = 30
        } else if (cmd == "hs_wall_blocks") {
            geometry::map_data map;
            map.linedefs.add(solid_wall({-50.0f, 50.0f}, {50.0f, 50.0f}));
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(map, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_wall_behind_actor ─────────────────────────────────────────
        // Solid wall behind the actor — must NOT block, actor gets hit.
        // Wall at y=200, actor at (0, 100). Shoot angle=0 (+Y).
        // Expected: HP = 20
        } else if (cmd == "hs_wall_behind_actor") {
            geometry::map_data map;
            map.linedefs.add(solid_wall({-50.0f, 200.0f}, {50.0f, 200.0f}));
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(map, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_portal_passes ─────────────────────────────────────────────
        // Two-sided wall (portal) between shooter and actor — must NOT block.
        // Portal at y=50, actor at (0, 100). Shoot angle=0 (+Y).
        // Expected: HP = 20
        } else if (cmd == "hs_portal_passes") {
            geometry::map_data map;
            map.linedefs.add(portal_wall({-50.0f, 50.0f}, {50.0f, 50.0f}));
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(map, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(mptr->hp());

        // ── hs_closest_wins ──────────────────────────────────────────────
        // Two actors on the same ray — only the closer one is hit.
        // A at (0, 50), B at (0, 100). Shoot angle=0 (+Y).
        // Expected: A hp=20, B hp=30
        } else if (cmd == "hs_closest_wins") {
            engine::world w;
            auto a = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 50.0f}, 0.0f);
            auto b = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* aptr = &*a;
            auto* bptr = &*b;
            w.register_entity(std::move(a));
            w.register_entity(std::move(b));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result(aptr->hp());
            result(bptr->hp());

        // ── hs_dead_skipped ──────────────────────────────────────────────
        // Dead actor must be skipped (take_damage not called again).
        // Kill monster, then fire. HP must stay at 0.
        // Expected: HP = 0
        } else if (cmd == "hs_dead_skipped") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            mptr->take_damage(999.0f);  // kill
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);  // should be skipped
            result(mptr->hp());

        // ── hs_angle_north ───────────────────────────────────────────────
        // Shoot west (angle = π/2 → dir = (-1, 0)), actor directly west.
        // Actor at (-100, 0). Shoot at π/2.
        // Expected: HP = 20
        } else if (cmd == "hs_angle_north") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{-100.0f, 0.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, std::numbers::pi_v<float> / 2.0f, 10.0f);
            result(mptr->hp());

        // ── hs_no_targets ────────────────────────────────────────────────
        // Empty world — must not crash.
        // Expected: "OK"
        } else if (cmd == "hs_no_targets") {
            engine::world empty_w;
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, empty_w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 10.0f);
            result("OK");

        // ── hs_damage_exact ──────────────────────────────────────────────
        // Verify exact damage value reaches the target.
        // monster_basic has 30 HP. Fire with damage=7 → HP = 23.
        // Expected: HP = 23
        } else if (cmd == "hs_damage_exact") {
            engine::world w;
            auto m = std::make_unique<inspect<entities::monster_basic>>(math::vec2{0.0f, 100.0f}, 0.0f);
            auto* mptr = &*m;
            w.register_entity(std::move(m));
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(empty_md, w);
            ammo->spawn_bullet(ORIGIN, 0.0f, 7.0f);
            result(mptr->hp());
        }
    }
    return 0;
}
