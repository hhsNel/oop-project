#include "weapon.h"
#include <algorithm>

namespace combat {
	namespace weapons {

		weapon::weapon(unsigned int id, std::unique_ptr<firing_mode> ammo_type, int max, float rate, float dmg, int reserve)
			: weapon_id(id), ammo(std::move(ammo_type)), ammo_count(max),
			  max_ammo(max), reserve_mags(reserve), fire_rate(rate), last_shot_time(0.0f), damage(dmg) {}

		bool weapon::can_fire() const {
			return ammo_count > 0 && last_shot_time <= 0.0f;
		}

		void weapon::update(float dt) {
			last_shot_time = std::max(0.0f, last_shot_time - dt);
			if (ammo) ammo->update(dt);
		}

		bool weapon::accepts_ammo(unsigned int ammo_weapon_id) const {
			return weapon_id == ammo_weapon_id;
		}

		void weapon::resupply(int mags) {
			reserve_mags += mags;
		}

		void weapon::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			ammo->spawn_bullet(pos, angle, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
		}

		void weapon::reload() {
			if (reserve_mags <= 0) return;
			--reserve_mags;
			ammo_count = max_ammo;
		}
	}
}
