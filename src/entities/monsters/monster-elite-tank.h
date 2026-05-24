#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_tank : public monster {
		bool  melee_mode;
		float melee_threshold;
		float heavy_timer;
		float heavy_cd;
	public:
		monster_elite_tank(math::vec2 const p, float const z)
			: monster(p, z, 6, 1.0f, 400.0f, 150.0f, 1.2f, 2.5f, 8.0f, 15.0f, 1.2f),
			  melee_mode(false), melee_threshold(3.0f),
			  heavy_timer(0.0f), heavy_cd(6.0f) {}
		void update(float dt) override;
	};

}
