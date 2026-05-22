#include <iostream>
#include <string>
#include <memory>
#include <numbers>

#include "combat/weapons/hitscan-firing-mode.h"
#include "combat/weapons/pistol.h"
#include "entities/monsters.h"
#include "geometry/map-data.h"
#include "geometry/linedef.h"
#include "geometry/sidedef.h"

// Exposes health fields for test inspection via componentized.
template<typename M>
struct inspect : public M {
    using M::M;
    float hp()   const { return this->health("current_hp"_f); }
    float armor() const { return this->health("armor"_f); }
};

static void result(float v) {
    std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(const std::string& s) {
    std::cout << "RESULT " << s << std::endl;
}

// Shorthand: fire one hitscan shot and return the ammo object for inspection.
static combat::weapons::hitscan_firing_mode*
fire(combat::weapons::hitscan_firing_mode* ammo,
     math::vec2 from, float angle, float damage = 10.0f)
{
    ammo->spawn_bullet(from, angle, damage);
    return ammo;
}

// Build a solid wall linedef (back = nullid).
static geometry::linedef solid_wall(math::vec2 a, math::vec2 b) {
    geometry::linedef ld;
    ld.v1   = a;
    ld.v2   = b;
    ld.front = 1;
    ld.back  = util::indexed_storage<geometry::sidedef>::nullid;
    return ld;
}

// Build a portal linedef (both sides set — passes through).
static geometry::linedef portal_wall(math::vec2 a, math::vec2 b) {
    geometry::linedef ld;
    ld.v1   = a;
    ld.v2   = b;
    ld.front = 1;
    ld.back  = 2;   // non-null → portal, hitscan passes through
    return ld;
}

int main() {
    std::cout << "READY" << std::endl;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        // ── hs_direct_hit ────────────────────────────────────────────────
        // Actor straight ahead — must be hit.
        // Setup: monster_basic (30 HP), from (0,0) east, actor at (100,0), dmg=10
        // Expected: HP = 20
        if (cmd == "hs_direct_hit") {
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_miss_side ─────────────────────────────────────────────────
        // Actor perpendicular to ray — must miss.
        // Actor at (0, 100), shoot east. Perpendicular distance >> hit_radius.
        // Expected: HP = 30
        } else if (cmd == "hs_miss_side") {
            inspect<entities::monster_basic> m;
            m.pos ={0.0f, 100.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_miss_behind ───────────────────────────────────────────────
        // Actor behind the shooter — must miss (t < 0).
        // Actor at (-100, 0), shoot east.
        // Expected: HP = 30
        } else if (cmd == "hs_miss_behind") {
            inspect<entities::monster_basic> m;
            m.pos ={-100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_miss_range ────────────────────────────────────────────────
        // Actor beyond max_range — must miss.
        // Actor at (600, 0), range = 500.
        // Expected: HP = 30
        } else if (cmd == "hs_miss_range") {
            inspect<entities::monster_basic> m;
            m.pos ={600.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr, 500.0f);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_hit_in_range ──────────────────────────────────────────────
        // Actor within max_range — must be hit.
        // Actor at (400, 0), range = 500.
        // Expected: HP = 20
        } else if (cmd == "hs_hit_in_range") {
            inspect<entities::monster_basic> m;
            m.pos ={400.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr, 500.0f);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_wall_blocks ───────────────────────────────────────────────
        // Solid wall between shooter and actor — actor must NOT be hit.
        // Wall at x=50, actor at (100, 0). Shoot east.
        // Expected: HP = 30
        } else if (cmd == "hs_wall_blocks") {
            geometry::map_data map;
            map.linedefs.add(solid_wall({50.0f, -50.0f}, {50.0f, 50.0f}));
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(&map);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_wall_behind_actor ─────────────────────────────────────────
        // Solid wall behind the actor — must NOT block, actor gets hit.
        // Wall at x=200, actor at (100, 0). Shoot east.
        // Expected: HP = 20
        } else if (cmd == "hs_wall_behind_actor") {
            geometry::map_data map;
            map.linedefs.add(solid_wall({200.0f, -50.0f}, {200.0f, 50.0f}));
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(&map);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_portal_passes ─────────────────────────────────────────────
        // Two-sided wall (portal) between shooter and actor — must NOT block.
        // Portal at x=50, actor at (100, 0). Shoot east.
        // Expected: HP = 20
        } else if (cmd == "hs_portal_passes") {
            geometry::map_data map;
            map.linedefs.add(portal_wall({50.0f, -50.0f}, {50.0f, 50.0f}));
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(&map);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(m.hp());

        // ── hs_closest_wins ──────────────────────────────────────────────
        // Two actors on the same ray — only the closer one is hit.
        // A at (50, 0), B at (100, 0). Shoot east.
        // Expected: A hp=20, B hp=30
        } else if (cmd == "hs_closest_wins") {
            inspect<entities::monster_basic> a, b;
            a.pos ={50.0f,  0.0f};
            b.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&a, &b});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result(a.hp());
            result(b.hp());

        // ── hs_dead_skipped ──────────────────────────────────────────────
        // Dead actor must be skipped (take_damage not called again).
        // Kill monster, then fire. HP must stay at 0.
        // Expected: HP = 0
        } else if (cmd == "hs_dead_skipped") {
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            m.take_damage(999.0f);  // kill
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);  // should be skipped
            result(m.hp());

        // ── hs_angle_north ───────────────────────────────────────────────
        // Shoot north (angle = π/2), actor directly north.
        // Actor at (0, 100). Shoot at π/2.
        // Expected: HP = 20
        } else if (cmd == "hs_angle_north") {
            inspect<entities::monster_basic> m;
            m.pos ={0.0f, 100.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, std::numbers::pi_v<float> / 2.0f);
            result(m.hp());

        // ── hs_no_targets ────────────────────────────────────────────────
        // Empty target list — must not crash.
        // Expected: "OK"
        } else if (cmd == "hs_no_targets") {
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f);
            result("OK");

        // ── hs_damage_exact ──────────────────────────────────────────────
        // Verify exact damage value reaches the target.
        // monster_basic has 30 HP. Fire with damage=7 → HP = 23.
        // Expected: HP = 23
        } else if (cmd == "hs_damage_exact") {
            inspect<entities::monster_basic> m;
            m.pos ={100.0f, 0.0f};
            auto ammo = std::make_unique<combat::weapons::hitscan_firing_mode>(nullptr);
            (*ammo)("targets"_f) = std::vector<engine::actor*>({&m});
            fire(ammo.get(), {0.0f, 0.0f}, 0.0f, 7.0f);
            result(m.hp());
        }
    }
    return 0;
}
