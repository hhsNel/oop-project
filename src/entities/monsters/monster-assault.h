#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_assault : public monster, public util::componentized<monster_assault> {
		[[=util::component_field{}]] int   burst_size;
		[[=util::component_field{}]] float burst_interval;
		int   burst_remaining;
		float burst_timer;
		float burst_cooldown;
		float strafe_sign;
		float strafe_timer;

		friend class util::componentized<monster_assault>;
	public:
		using util::componentized<monster_assault>::operator();
		using util::componentized<monster>::operator();

		monster_assault() : monster(60.0f, 10.0f, 2.0f, 6.0f, 12.0f, 7.0f, 0.2f),
		    burst_size(3), burst_interval(0.2f),
		    burst_remaining(0), burst_timer(0.0f), burst_cooldown(0.0f),
		    strafe_sign(1.0f), strafe_timer(0.0f) {}
		void update(float dt) override;
	};

}
