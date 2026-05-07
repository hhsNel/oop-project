#include "grenade-ammo.h"
#include "math/ray2.h"
#include "geometry/sidedef.h"
#include <cmath>

namespace combat {
    namespace weapons {

        grenade_ammunition::grenade_ammunition(geometry::map_data* m, float fuse, float radius, float speed)
            : map(m), fuse_time(fuse), explosion_radius(radius), throw_speed(speed) {}

        void grenade_ammunition::spawn_bullet(math::vec2 pos, float angle, float damage) {
            active.push_back({
                pos,
                {std::cos(angle) * throw_speed, std::sin(angle) * throw_speed},
                fuse_time,
                explosion_radius,
                damage
            });
        }

        void grenade_ammunition::update(float dt) {
            auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;

            for (auto it = active.begin(); it != active.end(); ) {
                live_grenade& g = *it;

                g.fuse_remaining -= dt;
                if (g.fuse_remaining <= 0.0f) {
                    explode(g);
                    it = active.erase(it);
                    continue;
                }

                // ── Move grenade; bounce off solid walls ──────────────────────────
                math::vec2 step     = g.velocity * dt;
                float      step_len = step.len();

                if (step_len > 1e-6f && map) {
                    float      min_dist     = step_len;
                    math::vec2 best_normal  = {0.0f, 0.0f};
                    bool       hit_wall     = false;

                    math::ray2 ray{g.position, step};

                    for (auto const& e : map->linedefs) {
                        geometry::linedef const& ld = e.value;
                        if (ld.back != null_sd) continue;   // portals don't block

                        math::vec2 hit;
                        float dist = 0.0f, seg_len = 0.0f;
                        if (!ray.intersects({ld.v1, ld.v2}, hit, dist, seg_len)) continue;
                        if (dist >= min_dist) continue;

                        // Compute inward-facing wall normal
                        math::vec2 wall_dir = (ld.v2 - ld.v1).normalized();
                        math::vec2 n        = wall_dir.perpendicular();
                        if (math::vec2::dot_product(n, g.velocity) > 0.0f) n = -n;

                        min_dist    = dist;
                        best_normal = n;
                        hit_wall    = true;
                    }

                    if (hit_wall) {
                        // Move up to wall
                        g.position = g.position + step.normalized() * min_dist;
                        // Reflect velocity and damp
                        float dot   = math::vec2::dot_product(g.velocity, best_normal);
                        g.velocity  = (g.velocity - best_normal * (2.0f * dot)) * bounce_damping;
                    } else {
                        g.position = g.position + step;
                    }
                } else {
                    g.position = g.position + step;
                }

                ++it;
            }
        }

        void grenade_ammunition::explode(live_grenade const& g) {
            for (engine::actor* a : targets) {
                if (!a || a->is_dead()) continue;
                math::vec2 offset = a->pos - g.position;
                float dist = offset.len();
                if (dist >= g.explosion_radius) continue;
                // Linear falloff: full damage at centre, zero at edge
                float falloff = 1.0f - dist / g.explosion_radius;
                a->take_damage(g.damage * falloff);
            }
        }
    }
}
