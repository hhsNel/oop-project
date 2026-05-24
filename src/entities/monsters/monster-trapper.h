#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_trapper : public monster {
	protected:
		int   max_traps;
		int   traps_placed;
	private:
		float trap_timer;
		float trap_interval;
		float wander_timer;
		float wander_sign;

	public:
		monster_trapper(math::vec2 const p, float const z)
			: monster(p, z, 12, 1.0f, 45.0f, 15.0f, 3.0f, 3.0f, 8.0f, 15.0f),
			  max_traps(3), traps_placed(0),
			  trap_timer(0.0f), trap_interval(2.0f),
			  wander_timer(0.0f), wander_sign(1.0f) {}
		void update(float dt) override;
	};

}
