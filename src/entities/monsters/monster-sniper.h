#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_sniper : public monster {
	protected:
		float shoot_interval;
		float aim_timer;

	public:
		monster_sniper(math::vec2 const p, float const z, assets::texture_id const tex, float const is)
			: monster(p, z, tex, is, 40.0f, 0.0f, 1.0f, 25.0f, 30.0f, 30.0f, 3.0f),
			  shoot_interval(3.0f), aim_timer(0.0f) {}
		void update(float dt) override;
	};

}
