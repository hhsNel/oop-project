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
		monster_assault(math::vec2 const p, float const z);
		void update(float dt) override;
	};

}
