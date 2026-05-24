#pragma once

#include <memory>

#include "math/vec2.h"
#include "firing-mode.h"
#include "ammo-type.h"

int main();

namespace combat
{
	namespace weapons {
		class weapon {
		protected:
			ammo_type accepted_ammo;
			std::unique_ptr<firing_mode> ammo;
			int ammo_count;
			int max_ammo;
			int reserve_mags;
			float fire_rate;
			float last_shot_time;
			float damage;

			friend int ::main();
		public:
			virtual bool can_fire() const;
			void tick(float dt);
			bool accepts_ammo(ammo_type type) const;
			void resupply(int amount);

			virtual void fire(math::vec2 pos, float angle);
			virtual void reload();
			virtual ~weapon() = default;

		protected:
			weapon(ammo_type type, std::unique_ptr<firing_mode> firing, int max, float rate, float dmg, int reserve = 0);
		};
	}
}
