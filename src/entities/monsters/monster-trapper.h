#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_trapper : public monster {
		/* maximum number of traps that can be active */
		int max_traps;
		/* number of traps currently placed */
		int traps_placed;
		/* time until the next trap can be placed */
		float trap_timer;
		/* time between placing traps */
		float trap_interval;
		/* time until the wander direction flips */
		float wander_timer;
		/* current wander direction (+1 or -1) */
		float wander_sign;

	public:
		/* constructor */
		monster_trapper(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
