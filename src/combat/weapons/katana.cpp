#include "katana.h"

namespace combat {
	namespace weapons {
		katana::katana()
			: weapon(ammo_type::none, nullptr, 0, 1.5f, 50.0f), swing_count(0) {}

		katana::katana(std::unique_ptr<firing_mode> fm)
			: weapon(ammo_type::none, std::move(fm), 0, 1.5f, 50.0f), swing_count(0) {}

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
