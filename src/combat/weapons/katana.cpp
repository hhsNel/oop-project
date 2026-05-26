#include "katana.h"

namespace combat {
	namespace weapons {
		katana::katana()
			: weapon(nullptr, 0, 1.5f, 50.0f) {}

		bool katana::can_fire() const {
			return last_shot_time <= 0.0f;
		}

		void katana::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			if (ammo) ammo->spawn_bullet(pos, angle, damage);
			last_shot_time = 1.0f / fire_rate;
		}

		void katana::reload() {}
	}
}
