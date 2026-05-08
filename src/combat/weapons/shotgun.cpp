#include "shotgun.h"

namespace combat {
	namespace weapons {
		shotgun::shotgun(std::unique_ptr<ammunition> ammo_type, int mag_size, int max, float rate, float dmg)
			: weapon(0, std::move(ammo_type), mag_size, max, rate, dmg) {}

		void shotgun::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			float step        = spread / static_cast<float>(pellet_count - 1);
			float start_angle = angle - spread / 2.0f;
			for (int i = 0; i < pellet_count; ++i)
				(*this)("ammo"_f)->spawn_bullet(pos, start_angle + static_cast<float>(i) * step, (*this)("damage"_f));
			--(*this)("ammo_count"_f);
			(*this)("last_shot_time"_f) = 1.0f / (*this)("fire_rate"_f);
		}

		void shotgun::reload() {
			(*this)("ammo_count"_f) = (*this)("max_ammo"_f);
		}
	}
}
