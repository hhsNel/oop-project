#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_swift : public monster {
		/* movement speed while charging */
		float charge_speed;
		/* whether the monster is currently charging */
		bool is_charging;
		/* time left in the current charge */
		float charge_timer;
		/* time until the next charge is available */
		float charge_cd;
		/* current angle while circling the target */
		float circle_angle;
		/* radius kept while circling the target */
		float circle_radius;

	public:
		/* constructor */
		monster_elite_swift(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
