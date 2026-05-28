#include "monster-maly-szybki.h"

namespace entities {

monster_Maly_Szybki::monster_Maly_Szybki(math::vec2 const p, float const z, engine::actor* target, geometry::map_data* map, engine::world* world)
	: monster(p, z, 8, 1.0f, 15.0f, 0.0f, 180.0f, 40.0f, 300.0f, 12.0f, 0.5f, target, map, world),
	  is_dashing(false), dash_timer(0.0f), dash_cooldown(0.0f) {}

void monster_Maly_Szybki::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dash_cooldown > 0.0f) { dash_cooldown -= dt; return; }

    if (is_dashing) {
        move_toward_target(movement_speed * 3.5f, dt);
        dash_timer -= dt;

        if (dist <= attack_range) {
            if (attack_cooldown <= 0.0f) {
                melee_attack(attack_damage);
                attack_cooldown = attack_cd_max;
            }
            is_dashing   = false;
            dash_cooldown = 2.5f;
        }
        if (dash_timer <= 0.0f) {
            is_dashing   = false;
            dash_cooldown = 2.5f;
        }
    } else {
        if (dist <= detection_radius) {
            is_dashing = true;
            dash_timer = 1.0f;
        }
    }
}

}
