#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_all_rounder : public monster {
		bool  melee_mode;
		float melee_threshold;

	public:
		monster_all_rounder(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 80.0f, 20.0f, 80.0f, 100.0f, 250.0f, 10.0f, 1.0f),
			  melee_mode(false), melee_threshold(60.0f) {}
		void update(float dt) override;
	};

}
