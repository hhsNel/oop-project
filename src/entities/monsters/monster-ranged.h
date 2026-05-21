#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_ranged : public monster {
		float preferred_dist;
	public:
		monster_ranged() : monster(25.0f, 0.0f, 1.2f, 10.0f, 15.0f, 8.0f, 1.5f),
		                   preferred_dist(8.0f) {}
		void update(float dt) override;
	};

}
