#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_assault : public monster {
		/* number of shots in a burst */
		int burst_size;
		/* time between shots within a burst */
		float burst_interval;
		/* shots left in the current burst */
		int burst_remaining;
		/* time until the next shot in the burst */
		float burst_timer;
		/* time until the next burst */
		float burst_cooldown;
		/* current strafing direction (+1 or -1) */
		float strafe_sign;
		/* time until the strafe direction flips */
		float strafe_timer;

	public:
		/* constructor */
		monster_assault(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
