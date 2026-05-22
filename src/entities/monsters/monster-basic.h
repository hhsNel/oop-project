#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_basic : public monster {
	public:
		monster_basic() : monster(100.0f, 0.0f, 1.5f, 1.5f, 8.0f, 10.0f, 1.0f) {}
		void update(float dt) override;
	};

}
