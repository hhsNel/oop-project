#include "monster-basic.h"

namespace entities {

void monster_basic::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dist > attack_range)
        move_toward_target(movement_speed, dt);

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
    }
}

}
