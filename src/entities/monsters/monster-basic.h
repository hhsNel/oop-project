#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_basic : public monster {
	public:
		monster_basic(math::vec2 const p, float const z)
			: monster(p, z, 2, 1.0f, 30.0f, 0.0f, 1.5f, 1.5f, 8.0f, 10.0f, 1.0f) {}
		void update(float dt) override;
	};

}
