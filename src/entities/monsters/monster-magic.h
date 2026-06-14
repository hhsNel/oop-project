#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_magic : public monster {
		float charge_time;
		float charge_timer;
		bool  is_charging;
		float post_fire_cooldown;
		float sidestep_timer;
		float sidestep_sign;

	public:
		/* constructor */
		monster_magic(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
