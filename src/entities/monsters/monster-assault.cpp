#include "monster-assault.h"

namespace entities {

monster_assault::monster_assault(math::vec2 const p, float const z, engine::actor* target, geometry::map_data* map, engine::world* world)
	: monster(p, z, 1, 1.0f, 60.0f, 10.0f, 120.0f, 150.0f, 300.0f, 7.0f, 0.2f, target, map, world),
	  burst_size(3), burst_interval(0.2f),
	  burst_remaining(0), burst_timer(0.0f), burst_cooldown(0.0f),
	  strafe_sign(1.0f), strafe_timer(0.0f) {}

void monster_assault::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    strafe_timer -= dt;
    if (strafe_timer <= 0.0f) {
        strafe_sign   = -strafe_sign;
        strafe_timer  = 1.2f;
    }
    strafe(movement_speed * 0.7f * strafe_sign, dt);

    float preferred = attack_range * 0.75f;
    if (dist > preferred + 1.5f) move_toward_target(movement_speed * 0.5f, dt);
    else if (dist < preferred - 1.5f) move_away_from_target(movement_speed * 0.5f, dt);

    if (burst_cooldown > 0.0f) { burst_cooldown -= dt; return; }

    if (burst_remaining > 0) {
        burst_timer -= dt;
        if (burst_timer <= 0.0f && dist <= attack_range) {
            ranged_attack(attack_damage, 30, 300.0f);
            --burst_remaining;
            burst_timer = burst_interval;
        }
        if (burst_remaining == 0)
            burst_cooldown = 2.5f;
    } else {
        burst_remaining = burst_size;
        burst_timer     = 0.0f;
    }
}

}
