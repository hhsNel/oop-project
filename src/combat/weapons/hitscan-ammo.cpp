#include "hitscan-ammo.h"
#include "math/ray2.h"
#include "geometry/sidedef.h"
#include <cmath>

namespace engine::combat {

hitscan_ammunition::hitscan_ammunition(geometry::map_data* m, float range, float radius)
    : map(m), max_range(range), hit_radius(radius) {}

void hitscan_ammunition::spawn_bullet(math::vec2 pos, float angle, float damage) {
    math::vec2 dir{std::cos(angle), std::sin(angle)};
    math::ray2 ray{pos, dir};

    // ── Closest solid-wall hit ────────────────────────────────────────────
    float wall_dist = max_range;

    if (map) {
        auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;
        for (auto const& e : map->linedefs) {
            geometry::linedef const& ld = e.value;
            if (ld.back != null_sd) continue;   // two-sided wall = portal, pass through

            math::vec2 hit;
            float dist = 0.0f, seg_len = 0.0f;
            if (ray.intersects({ld.v1, ld.v2}, hit, dist, seg_len) && dist < wall_dist)
                wall_dist = dist;
        }
    }

    // ── Closest actor hit (cylinder approximation) ────────────────────────
    // For each actor project its centre onto the ray; if the perpendicular
    // distance is within hit_radius and the projection is between the shooter
    // and the nearest wall, it counts as a hit.
    engine::actor* hit_target = nullptr;
    float          target_dist = wall_dist;

    for (engine::actor* a : targets) {
        if (!a || a->is_dead()) continue;

        math::vec2 to_actor = a->position - pos;
        float t = math::vec2::dot_product(to_actor, dir);
        if (t < 0.0f || t > target_dist) continue;

        math::vec2 closest = pos + dir * t;
        math::vec2 offset  = a->position - closest;
        if (offset.sqr_len() < hit_radius * hit_radius) {
            target_dist = t;
            hit_target  = a;
        }
    }

    if (hit_target)
        hit_target->take_damage(damage);
}

} // namespace engine::combat
