#pragma once

#include "pickup.h"

namespace world_object {

	class armor_pickup : public pickup {
		float armor_amount;
	public:
		armor_pickup(math::vec2 pos, float amount, float radius = 20.0f);
		void on_pickup(entities::player& p) override;
	};

}
