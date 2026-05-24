#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 200.0f, 50.0f, 0.8f, 2.0f, 6.0f, 25.0f, 2.0f) {}
		void update(float dt) override;
	};

}
