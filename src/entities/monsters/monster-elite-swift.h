#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_elite_swift : public monster {
		float charge_speed;
		bool  is_charging;
		float charge_timer;
		float charge_cd;
		float circle_angle;
		float circle_radius;

	public:
		monster_elite_swift(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		void update(float dt) override;
	};

}
