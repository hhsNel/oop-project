#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_sniper : public monster, public util::componentized<monster_sniper> {
		[[=util::component_field{}]] float shoot_interval;
		[[=util::component_field{}]] float aim_timer;

		friend class util::componentized<monster_sniper>;
	public:
		using util::componentized<monster_sniper>::operator();
		using util::componentized<monster>::operator();

		monster_sniper() : monster(40.0f, 0.0f, 1.0f, 25.0f, 30.0f, 30.0f, 3.0f),
		    shoot_interval(3.0f), aim_timer(0.0f) {}
		void update(float dt) override;
	};

}
