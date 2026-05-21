#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_magic : public monster, public util::componentized<monster_magic> {
		float charge_time;
		[[=util::component_field{}]] float charge_timer;
		[[=util::component_field{}]] bool  is_charging;
		float post_fire_cooldown;
		float sidestep_timer;
		float sidestep_sign;

		friend class util::componentized<monster_magic>;
	public:
		using util::componentized<monster_magic>::operator();
		using util::componentized<monster>::operator();

		monster_magic() : monster(50.0f, 30.0f, 1.5f, 12.0f, 15.0f, 40.0f, 2.0f),
		    charge_time(2.0f), charge_timer(0.0f), is_charging(false),
		    post_fire_cooldown(0.0f), sidestep_timer(0.0f), sidestep_sign(1.0f) {}
		void update(float dt) override;
	};

}
