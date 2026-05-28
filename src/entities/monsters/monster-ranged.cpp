#include "monster-ranged.h"

namespace entities {

void monster_ranged::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dist < preferred_dist - 1.0f)
        move_away_from_target(movement_speed, dt);
    else if (dist > preferred_dist + 1.0f)
        move_toward_target(movement_speed, dt);

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        ranged_attack(attack_damage, 30, 200.0f);
        attack_cooldown = attack_cd_max;
    }
}

}
