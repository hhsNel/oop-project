#include "monster-ranged.h"

namespace entities {

monster_ranged::monster_ranged(math::vec2 const p, float const z, engine::actor* target, geometry::map_data* map, engine::world* world)
	: monster(p, z, 9, 1.0f, 25.0f, 0.0f, 60.0f, 250.0f, 350.0f, 8.0f, 1.5f, target, map, world),
	  preferred_dist(200.0f) {}

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
