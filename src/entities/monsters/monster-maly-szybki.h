#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_Maly_Szybki : public monster {
		bool  is_dashing;
		float dash_timer;
		float dash_cooldown;
	public:
		monster_Maly_Szybki(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 15.0f, 0.0f, 4.5f, 1.5f, 12.0f, 12.0f, 0.5f),
			  is_dashing(false), dash_timer(0.0f), dash_cooldown(0.0f) {}
		void update(float dt) override;
	};

}
