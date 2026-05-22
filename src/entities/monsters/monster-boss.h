#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_boss : public monster, public util::componentized<monster_boss> {
		friend class util::componentized<monster_boss>;

		[[=util::component_field{}]] int phase_count;

	public:
		using util::componentized<monster_boss>::operator();
		using util::componentized<monster>::operator();

		monster_boss() : monster(1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
		    phase_count(3) {}
	};

}
