#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_swift : public monster {
		float charge_speed;
		bool  is_charging;
		float charge_timer;
		float charge_cd;
		float circle_angle;
		float circle_radius;

	public:
		monster_elite_swift(math::vec2 const p, float const z)
			: monster(p, z, 5, 1.0f, 60.0f, 0.0f, 7.0f, 2.0f, 20.0f, 12.0f, 0.8f),
			  charge_speed(15.0f), is_charging(false),
			  charge_timer(0.0f), charge_cd(0.0f),
			  circle_angle(0.0f), circle_radius(5.0f) {}
		void update(float dt) override;
	};

}
