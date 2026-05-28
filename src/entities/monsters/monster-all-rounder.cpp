#include "monster-all-rounder.h"

namespace entities {

monster_all_rounder::monster_all_rounder(math::vec2 const p, float const z, engine::actor* target, geometry::map_data* map, engine::world* world)
	: monster(p, z, 0, 1.0f, 80.0f, 20.0f, 80.0f, 100.0f, 250.0f, 10.0f, 1.0f, target, map, world),
	  melee_mode(false), melee_threshold(60.0f) {}

void monster_all_rounder::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
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
            ranged_attack(attack_damage, 30, 250.0f);
            attack_cooldown = attack_cd_max;
        }
    }
}

}
