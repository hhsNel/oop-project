#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_boss : public monster {
	protected:
		int phase_count;

	public:
		monster_boss() : monster(1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
		    phase_count(3) {}
	};

}
