#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_tank : public monster {
		/* whether the monster is currently in melee mode */
		bool melee_mode;
		/* distance below which it switches to melee */
		float melee_threshold;
		/* time until the next heavy attack */
		float heavy_timer;
		/* cooldown between heavy attacks */
		float heavy_cd;
	public:
		/* constructor */
		monster_elite_tank(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
