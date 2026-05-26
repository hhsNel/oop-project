#include "monster-elite-swift.h"
#include <cmath>

namespace entities {

void monster_elite_swift::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (charge_cd > 0.0f) charge_cd -= dt;

    if (is_charging) {
        move_toward_target(charge_speed, dt);
        charge_timer -= dt;

        if (dist <= attack_range) {
            if (attack_cooldown <= 0.0f) {
                melee_attack(attack_damage * 2.0f);
                attack_cooldown = attack_cd_max;
            }
            is_charging = false;
            charge_cd   = 5.0f;
        }
        if (charge_timer <= 0.0f) {
            is_charging = false;
            charge_cd   = 5.0f;
        }
        return;
    }

    circle_angle += movement_speed * dt;
    if (target_ptr) {
        pos("x"_f) = (*target_ptr)("pos"_f)("x"_f) + std::cos(circle_angle) * circle_radius;
        pos("y"_f) = (*target_ptr)("pos"_f)("y"_f) + std::sin(circle_angle) * circle_radius;
    }

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
    }

    if (charge_cd <= 0.0f) {
        is_charging  = true;
        charge_timer = 0.8f;
    }
}

}
