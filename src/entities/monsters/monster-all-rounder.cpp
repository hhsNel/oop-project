#include "monster-all-rounder.h"

namespace entities {

void monster_all_rounder::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    melee_mode = (dist < melee_threshold);

    if (melee_mode) {
        if (dist > attack_range)
            move_toward_target(movement_speed, dt);
        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage * 1.5f);
            attack_cooldown = attack_cd_max;
        }
    } else {
        float preferred = attack_range * 0.7f;
        if (dist < preferred - 1.0f) move_away_from_target(movement_speed, dt);
        else if (dist > preferred + 1.0f) move_toward_target(movement_speed, dt);

        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage);
            attack_cooldown = attack_cd_max;
        }
    }
}

}
