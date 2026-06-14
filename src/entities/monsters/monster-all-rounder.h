#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_all_rounder : public monster {
		/* whether the monster is currently in melee mode */
		bool melee_mode;
		/* distance below which it switches to melee */
		float melee_threshold;

	public:
		/* constructor */
		monster_all_rounder(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
