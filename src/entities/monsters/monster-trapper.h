#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_trapper : public monster, public util::componentized<monster_trapper> {
		[[=util::component_field{}]] int   max_traps;
		[[=util::component_field{}]] int   traps_placed;
		float trap_timer;
		[[=util::component_field{}]] float trap_interval;
		float wander_timer;
		float wander_sign;

		friend class util::componentized<monster_trapper>;
	public:
		using util::componentized<monster_trapper>::operator();
		using util::componentized<monster>::operator();

		monster_trapper() : monster(45.0f, 15.0f, 3.0f, 3.0f, 8.0f, 15.0f),
		    max_traps(3), traps_placed(0),
		    trap_timer(0.0f), trap_interval(2.0f),
		    wander_timer(0.0f), wander_sign(1.0f) {}
		void update(float dt) override;
	};

}
