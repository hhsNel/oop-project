#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_spawner : public monster {
		int   max_spawns;
		int   current_spawns;
		float spawn_interval;
		float spawn_timer;

	public:
		/* constructor */
		monster_spawner(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
