#include "monster-duzy-gruby.h"

namespace entities {

monster_Duzy_Gruby::monster_Duzy_Gruby(math::vec2 const p, float const z, engine::actor* target, geometry::map_data* map, engine::world* world)
	: monster(p, z, 4, 1.0f, 200.0f, 50.0f, 40.0f, 45.0f, 150.0f, 25.0f, 2.0f, target, map, world) {}

void monster_Duzy_Gruby::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
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
