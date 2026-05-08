#include "katana.h"

namespace combat {
	namespace weapons {
		katana::katana(float rate, float dmg)
			: weapon(0, nullptr, 0, 0, rate, dmg), swing_count(0) {}

		katana::katana(std::unique_ptr<ammunition> ammo_type, float rate, float dmg)
			: weapon(0, std::move(ammo_type), 0, 0, rate, dmg), swing_count(0) {}

		bool katana::can_fire() const {
			return (*this)("last_shot_time"_f) <= 0.0f;
		}

		void katana::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			++swing_count;
			if ((*this)("ammo"_f)) (*this)("ammo"_f)->spawn_bullet(pos, angle, (*this)("damage"_f));
			(*this)("last_shot_time"_f) = 1.0f / (*this)("fire_rate"_f);
		}

		void katana::reload() {}
	}
}
