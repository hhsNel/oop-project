#pragma once

#include "pickup.h"

namespace world_object {

	class armor_pickup : public pickup {
		float armor_amount;
	public:
		armor_pickup(math::vec2 pos, float amount, entities::player& p, engine::world& w,
		             util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
		             float radius = 20.0f);
		void on_pickup(entities::player& p) override;
	};

}
