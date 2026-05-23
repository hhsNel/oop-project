#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_boss : public monster {
	protected:
		int phase_count;

	public:
		monster_boss(math::vec2 const p, float const z, assets::texture_id const tex, float const is)
			: monster(p, z, tex, is, 1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
			  phase_count(3) {}
	};

}
