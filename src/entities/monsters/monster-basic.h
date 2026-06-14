#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_basic : public monster {
	public:
		/* constructor */
		monster_basic(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
