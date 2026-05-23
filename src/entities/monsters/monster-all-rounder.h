#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_all_rounder : public monster {
	protected:
		bool  melee_mode;
	private:
		float melee_threshold;

	public:
		monster_all_rounder(math::vec2 const p, float const z, assets::texture_id const tex, float const is)
			: monster(p, z, tex, is, 80.0f, 20.0f, 2.0f, 5.0f, 10.0f, 10.0f, 1.0f),
			  melee_mode(false), melee_threshold(2.5f) {}
		void update(float dt) override;
	};

}
