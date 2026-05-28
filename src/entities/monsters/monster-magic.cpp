#include "monster-magic.h"

namespace entities {

void monster_magic::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (post_fire_cooldown > 0.0f) {
        post_fire_cooldown -= dt;
        strafe(movement_speed * sidestep_sign, dt);
        return;
    }

    if (!is_charging) {
        is_charging = true;
        charge_timer = 0.0f;
    }

    sidestep_timer -= dt;
    if (sidestep_timer <= 0.0f) {
        sidestep_sign  = -sidestep_sign;
        sidestep_timer = 0.8f;
    }
    strafe(movement_speed * 0.4f * sidestep_sign, dt);

    charge_timer += dt;
    if (charge_timer >= charge_time && dist <= attack_range && attack_cooldown <= 0.0f) {
        ranged_attack(attack_damage, 30, 200.0f);
        attack_cooldown    = attack_cd_max;
        is_charging        = false;
        charge_timer       = 0.0f;
        post_fire_cooldown = 3.0f;
    }
}

}
