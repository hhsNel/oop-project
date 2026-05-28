#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_tank : public monster {
		bool  melee_mode;
		float melee_threshold;
		float heavy_timer;
		float heavy_cd;
	public:
		monster_elite_tank(math::vec2 const p, float const z);
		void update(float dt) override;
	};

}
