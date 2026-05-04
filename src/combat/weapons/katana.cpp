#include "katana.h"

namespace engine::combat {
	katana::katana(float rate, float dmg)
		: weapon(0, nullptr, 0, 0, rate, dmg), swing_count(0) {}

	bool katana::can_fire() const {
		return last_shot_time <= 0.0f;
	}

	void katana::fire(math::vec2 /*pos*/, float /*angle*/) {
		if (!can_fire()) return;
		++swing_count;
		last_shot_time = 1.0f / fire_rate;
	}

	void katana::reload() {}
}
