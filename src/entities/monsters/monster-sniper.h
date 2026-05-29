#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_sniper : public monster {
		float shoot_interval;
		float aim_timer;

	public:
		monster_sniper(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
