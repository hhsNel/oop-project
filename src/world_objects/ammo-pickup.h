#pragma once

#include "pickup.h"

namespace combat { namespace weapons { enum class ammo_type : unsigned int; } }

namespace world_object {

	class ammo_pickup : public pickup {
		combat::weapons::ammo_type type;
		int amount;
	public:
		ammo_pickup(math::vec2 pos, combat::weapons::ammo_type type, int amt, float radius = 20.0f);
		void on_pickup(entities::player& p) override;
	};

}
