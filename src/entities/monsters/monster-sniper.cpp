#include "monster-sniper.h"

namespace entities {

void monster_sniper::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    aim_timer += dt;
    if (aim_timer >= shoot_interval && dist <= attack_range && attack_cooldown <= 0.0f) {
        ranged_attack(attack_damage, 29, 350.0f);
        attack_cooldown = attack_cd_max;
        aim_timer       = 0.0f;
    }
}

}
