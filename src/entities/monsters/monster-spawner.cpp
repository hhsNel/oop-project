#include "monster-spawner.h"

namespace entities {

monster_spawner::monster_spawner(math::vec2 const p, float const z)
	: monster(p, z, 11, 1.0f, 100.0f, 20.0f, 0.0f, 0.0f, 350.0f, 0.0f, 0.0f),
	  max_spawns(5), current_spawns(0),
	  spawn_interval(5.0f), spawn_timer(0.0f) {}

void monster_spawner::update(float dt) {
    monster::update(dt);
    if (is_dead()) return;
    if (current_spawns >= max_spawns) return;

    spawn_timer -= dt;
    if (spawn_timer <= 0.0f) {
        ++current_spawns;
        spawn_timer = spawn_interval;
    }
}

}
