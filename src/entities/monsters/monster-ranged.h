#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_ranged : public monster {
		float preferred_dist;
	public:
		monster_ranged(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
