#include "monster-magic.h"

namespace entities {

monster_magic::monster_magic(math::vec2 const p, float const z, engine::actor& tgt, geometry::map_data& map, engine::world& wrld)
	: monster(p, z, 7, 1.0f, 50.0f, 30.0f, 60.0f, 250.0f, 350.0f, 40.0f, 2.0f, tgt, map, wrld),
	  charge_time(2.0f), charge_timer(0.0f), is_charging(false),
	  post_fire_cooldown(0.0f), sidestep_timer(0.0f), sidestep_sign(1.0f) {}

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
