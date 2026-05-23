#include "weapon.h"
#include <algorithm>

namespace combat {
	namespace weapons {

		weapon::weapon(unsigned int id, std::unique_ptr<firing_mode> ammo_type, int max, float rate, float dmg)
			: weapon_id(id), ammo(std::move(ammo_type)), ammo_count(max),
			  max_ammo(max), fire_rate(rate), last_shot_time(0.0f), damage(dmg) {}

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

		void weapon::resupply(int amount) {
			ammo_count = std::min(ammo_count + amount, max_ammo);
		}

		void weapon::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			ammo->spawn_bullet(pos, angle, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
		}

		void weapon::reload() {
			ammo_count = max_ammo;
		}
	}
}
