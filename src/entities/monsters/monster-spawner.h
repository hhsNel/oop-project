#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_spawner : public monster {
	protected:
		int   max_spawns;
		int   current_spawns;
		float spawn_interval;
	private:
		float spawn_timer;

	public:
		monster_spawner(math::vec2 const p, float const z)
			: monster(p, z, 11, 1.0f, 100.0f, 20.0f, 0.0f, 0.0f, 15.0f),
			  max_spawns(5), current_spawns(0),
			  spawn_interval(5.0f), spawn_timer(0.0f) {}
		void update(float dt) override;
	};

}
