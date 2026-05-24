#pragma once

#include "pickup.h"

namespace world_object {

	class health_pickup : public pickup {
		float heal_amount;
	public:
		health_pickup(math::vec2 pos, float amount, float radius = 20.0f);
		void on_pickup(entities::player& p) override;
	};

}
