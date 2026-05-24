#pragma once

#include "pickup.h"
#include "entities/player.h"

namespace world_object {

	template<typename WeaponT>
	class ammo_pickup : public pickup {
		int amount;
	public:
		ammo_pickup(math::vec2 pos, int amt, float radius = 20.0f)
			: pickup(pos, radius), amount(amt) {}

		void on_pickup(entities::player& p) override {
			if (p.resupply<WeaponT>(amount))
				collected = true;
		}
	};

}
