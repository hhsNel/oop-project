#include "monster-trapper.h"

namespace entities {

void monster_trapper::update(float dt) {
    monster::update(dt);
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
