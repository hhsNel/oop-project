#include "monster-elite-swift.h"
#include "geometry/map-data.h"
#include <cmath>

namespace entities {

monster_elite_swift::monster_elite_swift(math::vec2 const p, float const z, engine::actor& tgt, geometry::map_data& map, engine::world& wrld)
	: monster(p, z, 5, 1.0f, 60.0f, 0.0f, 150.0f, 45.0f, 400.0f, 12.0f, 0.8f, tgt, map, wrld),
	  charge_speed(300.0f), is_charging(false),
	  charge_timer(0.0f), charge_cd(0.0f),
	  circle_angle(0.0f), circle_radius(120.0f) {}

void monster_elite_swift::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
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
    math::vec2 new_pos{
        target("pos"_f)("x"_f) + std::cos(circle_angle) * circle_radius,
        target("pos"_f)("y"_f) + std::sin(circle_angle) * circle_radius
    };
    apply_wall_collision(new_pos);
    map_ref.move_to(this, new_pos);

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
