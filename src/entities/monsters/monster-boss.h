#pragma once

#include "entities/monster.h"

int main();

namespace entities {

	class monster_boss : public monster {
		int phase_count;

		friend int ::main();

	public:
		monster_boss(math::vec2 const p, float const z)
			: monster(p, z, 3, 1.0f, 1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
			  phase_count(3) {}
	};

}
