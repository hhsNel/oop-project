#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_sniper : public monster {
	protected:
		float shoot_interval;
		float aim_timer;

	public:
		monster_sniper(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 40.0f, 0.0f, 1.0f, 25.0f, 30.0f, 30.0f, 3.0f),
			  shoot_interval(3.0f), aim_timer(0.0f) {}
		void update(float dt) override;
	};

}
