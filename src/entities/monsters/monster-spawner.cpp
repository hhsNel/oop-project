#include "monster-spawner.h"

namespace entities {

void monster_spawner::update(float dt) {
    monster::update(dt);
    if (current_spawns >= max_spawns) return;

    spawn_timer -= dt;
    if (spawn_timer <= 0.0f) {
        ++current_spawns;
        spawn_timer = spawn_interval;
    }
}

}
