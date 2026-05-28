#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby(math::vec2 const p, float const z)
			: monster(p, z, 4, 1.0f, 200.0f, 50.0f, 40.0f, 45.0f, 150.0f, 25.0f, 2.0f) {}
		void update(float dt) override;
	};

}
