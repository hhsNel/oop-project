#include "plasma-projectile.h"
#include "slug-projectile.h"
#include "geometry/map-data.h"
#include "geometry/linedef.h"
#include "geometry/sidedef.h"
#include "math/ray2.h"
#include <cmath>

namespace entities {

    projectile::projectile(math::vec2 start, float angle, float dmg,
                           float spd, float radius, float range,
                           geometry::map_data* m, std::vector<engine::actor*>* tgts,
                           assets::texture_id tex, float scale)
        : renderable_entity(start, 0.0f, tex, scale),
          velocity{std::cos(angle) * spd, std::sin(angle) * spd},
          damage(dmg), speed(spd), hit_radius(radius),
          max_range(range), traveled(0.0f), alive(true),
          map(m), targets(tgts) {}

    bool projectile::check_wall_collision(math::vec2 const& new_pos) const {
        if (!map) return false;

        auto null_sd = util::indexed_storage<geometry::sidedef>::nullid;
        math::vec2 dir = new_pos - pos;
        float move_dist = dir.len();
        if (move_dist < 0.001f) return false;

        math::ray2 ray{pos, dir * (1.0f / move_dist)};

        for (auto const& e : map->linedefs) {
            geometry::linedef const& ld = e.value;
            if (ld.back != null_sd) continue;

            math::vec2 hit;
            float dist = 0.0f, seg_len = 0.0f;
            if (ray.intersects({ld.v1, ld.v2}, hit, dist, seg_len) && dist <= move_dist)
                return true;
        }
        return false;
    }

    engine::actor* projectile::check_actor_collision(math::vec2 const& new_pos) const {
        if (!targets) return nullptr;

        math::vec2 dir = new_pos - pos;
        float move_dist = dir.len();

        for (engine::actor* a : *targets) {
            if (!a || a->is_dead()) continue;

            if (move_dist < 0.001f) {
                // Stationary check
                math::vec2 to_actor = a->pos - new_pos;
                if (to_actor.sqr_len() < hit_radius * hit_radius)
                    return a;
            } else {
                // Swept cylinder check along movement path (same as hitscan)
                math::vec2 norm_dir = dir * (1.0f / move_dist);
                math::vec2 to_actor = a->pos - pos;
                float t = math::vec2::dot_product(to_actor, norm_dir);
                t = std::clamp(t, 0.0f, move_dist);

                math::vec2 closest = pos + norm_dir * t;
                math::vec2 offset  = a->pos - closest;
                if (offset.sqr_len() < hit_radius * hit_radius)
                    return a;
            }
        }
        return nullptr;
    }

    void projectile::update(float dt) {
        if (!alive) return;

        math::vec2 new_pos = pos + velocity * dt;
        float step = speed * dt;
        traveled += step;

        if (traveled >= max_range) {
            alive = false;
            return;
        }

        if (check_wall_collision(new_pos)) {
            alive = false;
            return;
        }

        engine::actor* hit = check_actor_collision(new_pos);
        if (hit) {
            hit->take_damage(damage);
            alive = false;
            return;
        }

        if (map)
            map->move_to(this, new_pos);
        else
            pos = new_pos;
    }

    // ── Concrete projectile types ────────────────────────────────────────────

    plasma_projectile::plasma_projectile(math::vec2 start, float angle, float dmg,
                                         geometry::map_data* m, std::vector<engine::actor*>* tgts)
        : projectile(start, angle, dmg,
                     400.0f,   // speed
                     32.0f,    // hit_radius
                     800.0f,   // max_range
                     m, tgts,
                     0,        // tex_id (sprite texture 0)
                     0.15f)    // inherent_scale
    {}

    slug_projectile::slug_projectile(math::vec2 start, float angle, float dmg,
                                     geometry::map_data* m, std::vector<engine::actor*>* tgts)
        : projectile(start, angle, dmg,
                     300.0f,   // speed
                     16.0f,    // hit_radius
                     600.0f,   // max_range
                     m, tgts,
                     0,        // tex_id
                     0.1f)     // inherent_scale
    {}

}
