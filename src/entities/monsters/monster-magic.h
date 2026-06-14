#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_magic : public monster {
		/* time needed to fully charge a spell */
		float charge_time;
		/* time spent charging the current spell */
		float charge_timer;
		/* whether a spell is currently being charged */
		bool is_charging;
		/* cooldown after firing a spell */
		float post_fire_cooldown;
		/* time until the sidestep direction flips */
		float sidestep_timer;
		/* current sidestep direction (+1 or -1) */
		float sidestep_sign;

	public:
		/* constructor */
		monster_magic(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
