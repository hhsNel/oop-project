#pragma once
#define WEAPON_H

#include <algorithm>
#include <memory>

#include "math/vec2.h"
#include "ammunition.h"
#include "util/componentized.h"

namespace combat
{
	namespace weapons {
		class weapon : public util::componentized<weapon> {
			friend class util::componentized<weapon>;
		protected:
		/*zmiana z private na protected by przywrocic widocznosc do podklas
			aby nie korzystaly z componentized ktore powinno byc wykorzystane jedynie do eksportu na zewnatrz hierarchi klasy*/
			[[=util::component_field{}]] unsigned int weapon_id;
			[[=util::component_field{}]] std::unique_ptr<ammunition> ammo;
			[[=util::component_field{}]] int ammo_count;
			[[=util::component_field{}]] int max_ammo;
			[[=util::component_field{}]] float fire_rate;
			[[=util::component_field{}]] float last_shot_time;
			[[=util::component_field{}]] float damage;
		public:
			virtual bool can_fire() const { return ammo_count > 0 && last_shot_time <= 0.0f; }
			void update(float dt) {
				last_shot_time = std::max(0.0f, last_shot_time - dt);
				if (ammo) ammo->update(dt);
			}
			/*
			Metody domenowe zamiast get/set — umozliwiaja dostep zewnetrzny (np. world_objects)
			bez uzycia componentized, zgodnie z zasada enkapsulacji
			*/
			bool accepts_ammo(unsigned int ammo_weapon_id) const { return weapon_id == ammo_weapon_id; }
			void resupply(int amount) { ammo_count = std::min(ammo_count + amount, max_ammo); }

			virtual void fire(math::vec2 const pos, float const angle) = 0;
			virtual void reload() = 0;
			virtual ~weapon() = default;

		protected:
			weapon(unsigned int id, std::unique_ptr<ammunition> ammo_type, int mag, int max, float rate, float dmg)
				: weapon_id(id), ammo(std::move(ammo_type)), ammo_count(mag),
				max_ammo(max), fire_rate(rate), last_shot_time(0.0f), damage(dmg) {}
		};
	}
}

