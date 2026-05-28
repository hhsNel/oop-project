#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby(math::vec2 const p, float const z, engine::actor* target = nullptr, geometry::map_data* map = nullptr, engine::world* world = nullptr);
		void update(float dt) override;
	};

}
