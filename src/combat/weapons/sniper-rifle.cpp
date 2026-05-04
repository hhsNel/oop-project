#include "sniper-rifle.h"

namespace engine::combat {
	sniper_rifle::sniper_rifle(std::unique_ptr<ammunition> ammo_type, int mag_size, int max, float rate, float dmg)
		: weapon(0, std::move(ammo_type), mag_size, max, rate, dmg) {}

	void sniper_rifle::fire(math::vec2 pos, float angle) {
		if (!can_fire()) return;
		ammo->spawn_bullet(pos, angle, damage);
		--ammo_count;
		last_shot_time = 1.0f / fire_rate;
	}

	void sniper_rifle::reload() {
		ammo_count = max_ammo;
	}
}
