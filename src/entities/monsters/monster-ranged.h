#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_ranged : public monster {
		float preferred_dist;
	public:
		monster_ranged(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 25.0f, 0.0f, 1.2f, 10.0f, 15.0f, 8.0f, 1.5f),
			  preferred_dist(8.0f) {}
		void update(float dt) override;
	};

}
