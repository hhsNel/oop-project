#include "monster-sniper.h"

namespace entities {

monster_sniper::monster_sniper(math::vec2 const p, float const z, engine::actor& tgt, geometry::map_data& map, engine::world& wrld)
	: monster(p, z, 10, 1.0f, 40.0f, 0.0f, 50.0f, 500.0f, 600.0f, 30.0f, 3.0f, tgt, map, wrld),
	  shoot_interval(3.0f), aim_timer(0.0f) {}

void monster_sniper::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    aim_timer += dt;
    if (aim_timer >= shoot_interval && dist <= attack_range && attack_cooldown <= 0.0f) {
        ranged_attack(attack_damage, 30, 350.0f);
        attack_cooldown = attack_cd_max;
        aim_timer       = 0.0f;
    }
}

}
