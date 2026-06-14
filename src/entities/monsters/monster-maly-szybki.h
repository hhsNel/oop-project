#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_Maly_Szybki : public monster {
		bool  is_dashing;
		float dash_timer;
		float dash_cooldown;
	public:
		/* constructor */
		monster_Maly_Szybki(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);
		/* entity tick */
		void update(float dt) override;
	};

}
