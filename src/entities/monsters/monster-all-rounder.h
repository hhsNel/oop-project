#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_all_rounder : public monster, public util::componentized<monster_all_rounder> {
		[[=util::component_field{}]] bool  melee_mode;
		float melee_threshold;

		friend class util::componentized<monster_all_rounder>;
	public:
		using util::componentized<monster_all_rounder>::operator();
		using util::componentized<monster>::operator();

		monster_all_rounder() : monster(80.0f, 20.0f, 2.0f, 5.0f, 10.0f, 10.0f, 1.0f),
		    melee_mode(false), melee_threshold(2.5f) {}
		void update(float dt) override;
	};

}
