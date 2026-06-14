#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_assault : public monster {
		int   burst_size;
		float burst_interval;
		int   burst_remaining;
		float burst_timer;
		float burst_cooldown;
		float strafe_sign;
		float strafe_timer;

	public:
		/* constructor */
		monster_assault(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
