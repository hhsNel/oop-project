#include "monster-trapper.h"

namespace entities {

monster_trapper::monster_trapper(math::vec2 const p, float const z, engine::actor& tgt, geometry::map_data& map, engine::world& wrld)
	: monster(p, z, 12, 1.0f, 45.0f, 15.0f, 100.0f, 60.0f, 200.0f, 15.0f, 1.0f, tgt, map, wrld),
	  max_traps(3), traps_placed(0),
	  trap_timer(0.0f), trap_interval(2.0f),
	  wander_timer(0.0f), wander_sign(1.0f) {}

void monster_trapper::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    wander_timer -= dt;
    if (wander_timer <= 0.0f) {
        wander_sign  = -wander_sign;
        wander_timer = 1.5f;
    }
    strafe(movement_speed * wander_sign, dt);
    move_toward_target(movement_speed * 0.3f, dt);

    if (traps_placed < max_traps) {
        trap_timer -= dt;
        if (trap_timer <= 0.0f) {
            ++traps_placed;
            trap_timer = trap_interval;
        }
    }
}

}
