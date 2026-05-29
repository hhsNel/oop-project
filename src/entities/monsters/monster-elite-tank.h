#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_tank : public monster {
		bool  melee_mode;
		float melee_threshold;
		float heavy_timer;
		float heavy_cd;
	public:
		monster_elite_tank(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
