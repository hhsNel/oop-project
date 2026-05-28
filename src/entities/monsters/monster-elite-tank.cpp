#include "monster-elite-tank.h"

namespace entities {

void monster_elite_tank::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    heavy_timer -= dt;
    melee_mode = (dist < melee_threshold);

    if (melee_mode) {
        if (dist > attack_range) move_toward_target(movement_speed, dt);
        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage * 1.5f);
            attack_cooldown = attack_cd_max;
        }
    } else {
        float preferred = attack_range * 0.7f;
        if (dist < preferred - 1.0f) move_away_from_target(movement_speed, dt);
        else if (dist > preferred + 1.0f) move_toward_target(movement_speed, dt);

        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            float dmg = (heavy_timer <= 0.0f) ? attack_damage * 3.0f : attack_damage;
            melee_attack(dmg);
            attack_cooldown = attack_cd_max;
            if (heavy_timer <= 0.0f) heavy_timer = heavy_cd;
        }
    }
}

}
