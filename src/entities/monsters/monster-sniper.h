#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_sniper : public monster {
		/* time between shots */
		float shoot_interval;
		/* time spent aiming before the current shot */
		float aim_timer;

	public:
		/* constructor */
		monster_sniper(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
