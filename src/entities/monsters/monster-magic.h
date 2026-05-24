#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_magic : public monster {
		float charge_time;
		float charge_timer;
		bool  is_charging;
		float post_fire_cooldown;
		float sidestep_timer;
		float sidestep_sign;

	public:
		monster_magic(math::vec2 const p, float const z)
			: monster(p, z, 0, 1.0f, 50.0f, 30.0f, 1.5f, 12.0f, 15.0f, 40.0f, 2.0f),
			  charge_time(2.0f), charge_timer(0.0f), is_charging(false),
			  post_fire_cooldown(0.0f), sidestep_timer(0.0f), sidestep_sign(1.0f) {}
		void update(float dt) override;
	};

}
