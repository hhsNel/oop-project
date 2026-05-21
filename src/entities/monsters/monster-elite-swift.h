#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_elite_swift : public monster, public util::componentized<monster_elite_swift> {
		[[=util::component_field{}]] float charge_speed;
		bool  is_charging;
		float charge_timer;
		[[=util::component_field{}]] float charge_cd;
		float circle_angle;
		float circle_radius;

		friend class util::componentized<monster_elite_swift>;
	public:
		using util::componentized<monster_elite_swift>::operator();
		using util::componentized<monster>::operator();

		monster_elite_swift() : monster(60.0f, 0.0f, 7.0f, 2.0f, 20.0f, 12.0f, 0.8f),
		    charge_speed(15.0f), is_charging(false),
		    charge_timer(0.0f), charge_cd(0.0f),
		    circle_angle(0.0f), circle_radius(5.0f) {}
		void update(float dt) override;
	};

}
