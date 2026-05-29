#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_trapper : public monster {
		int   max_traps;
		int   traps_placed;
		float trap_timer;
		float trap_interval;
		float wander_timer;
		float wander_sign;

	public:
		monster_trapper(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
