#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_assault : public monster {
	protected:
		int   burst_size;
		float burst_interval;
	private:
		int   burst_remaining;
		float burst_timer;
		float burst_cooldown;
		float strafe_sign;
		float strafe_timer;

	public:
		monster_assault(math::vec2 const p, float const z, assets::texture_id const tex, float const is)
			: monster(p, z, tex, is, 60.0f, 10.0f, 2.0f, 6.0f, 12.0f, 7.0f, 0.2f),
			  burst_size(3), burst_interval(0.2f),
			  burst_remaining(0), burst_timer(0.0f), burst_cooldown(0.0f),
			  strafe_sign(1.0f), strafe_timer(0.0f) {}
		void update(float dt) override;
	};

}
