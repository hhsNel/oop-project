#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_all_rounder : public monster {
		bool  melee_mode;
		float melee_threshold;

	public:
		monster_all_rounder(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
