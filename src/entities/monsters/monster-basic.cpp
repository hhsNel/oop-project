#include "monster-basic.h"

namespace entities {

monster_basic::monster_basic(math::vec2 const p, float const z, engine::actor& tgt, geometry::map_data& map, engine::world& wrld)
	: monster(p, z, 2, 1.0f, 30.0f, 0.0f, 100.0f, 40.0f, 200.0f, 10.0f, 1.0f, tgt, map, wrld) {}

void monster_basic::update(float dt) {
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
