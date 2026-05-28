#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_sniper : public monster {
		float shoot_interval;
		float aim_timer;

	public:
		monster_sniper(math::vec2 const p, float const z)
			: monster(p, z, 10, 1.0f, 40.0f, 0.0f, 40.0f, 500.0f, 600.0f, 30.0f, 3.0f),
			  shoot_interval(3.0f), aim_timer(0.0f) {}
		void update(float dt) override;
	};

}
