#include "katana.h"

namespace combat {
	namespace weapons {
		katana::katana(float rate, float dmg)
			: weapon(0, nullptr, 0, 0, rate, dmg), swing_count(0) {}

		katana::katana(std::unique_ptr<firing_mode> ammo_type, float rate, float dmg)
			: weapon(0, std::move(ammo_type), 0, 0, rate, dmg), swing_count(0) {}

		bool katana::can_fire() const {
			return last_shot_time <= 0.0f;
		}

		void katana::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			++swing_count;
			if (ammo) ammo->spawn_bullet(pos, angle, damage);
			last_shot_time = 1.0f / fire_rate;
		}

		void katana::reload() {}
	}
}
