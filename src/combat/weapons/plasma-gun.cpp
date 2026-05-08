#include "plasma-gun.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(std::unique_ptr<ammunition> ammo_type, int mag_size, int max, float rate, float dmg)
			: weapon(0, std::move(ammo_type), mag_size, max, rate, dmg) {}

		void plasma_gun::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			(*this)("ammo"_f)->spawn_bullet(pos, angle, (*this)("damage"_f));
			--(*this)("ammo_count"_f);
			(*this)("last_shot_time"_f) = 1.0f / (*this)("fire_rate"_f);
		}

		void plasma_gun::reload() {
			(*this)("ammo_count"_f) = (*this)("max_ammo"_f);
		}
	}
}
