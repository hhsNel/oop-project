#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_spawner : public monster {
		/* maximum number of minions alive at once */
		int max_spawns;
		/* number of minions currently spawned */
		int current_spawns;
		/* time between spawns */
		float spawn_interval;
		/* time until the next spawn */
		float spawn_timer;

	public:
		/* constructor */
		monster_spawner(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
