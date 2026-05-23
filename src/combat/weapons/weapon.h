#pragma once
#define WEAPON_H

#include <memory>

#include "math/vec2.h"
#include "firing-mode.h"
#include "util/componentized.h"

namespace combat
{
	namespace weapons {
		class weapon : public util::componentized<weapon> {
			friend class util::componentized<weapon>;
		protected:
		/*zmiana z private na protected by przywrocic widocznosc do podklas
			aby nie korzystaly z componentized ktore powinno byc wykorzystane jedynie do eksportu na zewnatrz hierarchi klasy*/
			unsigned int weapon_id;
			std::unique_ptr<firing_mode> ammo;
			[[=util::component_field{}]] int ammo_count;
			[[=util::component_field{}]] int max_ammo;
			[[=util::component_field{}]] int reserve_mags;
			[[=util::component_field{}]] float fire_rate;
			[[=util::component_field{}]] float last_shot_time;
			[[=util::component_field{}]] float damage;
		public:
			virtual bool can_fire() const;
			void update(float dt);
			bool accepts_ammo(unsigned int ammo_weapon_id) const;
			void resupply(int amount);

			virtual void fire(math::vec2 pos, float angle);
			virtual void reload();
			virtual ~weapon() = default;

		protected:
			weapon(unsigned int id, std::unique_ptr<firing_mode> ammo_type, int max, float rate, float dmg, int reserve = 0);
		};
	}
}
