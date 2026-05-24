#pragma once

#include "pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"

namespace world_object {

	template<typename WeaponT>
	class ammo_pickup : public pickup {
		int amount;
	public:
		ammo_pickup(math::vec2 pos, int amt, float radius = 20.0f)
			: pickup(pos, radius), amount(amt) {}

		void on_pickup(entities::player& p) override {
			for (combat::weapons::weapon* w : p.weapons) {
				if (!w) continue;
				if (dynamic_cast<WeaponT*>(w)) {
					w->resupply(amount);
					collected = true;
					return;
				}
			}
		}
	};

}
