#pragma once

#include "pickup.h"

namespace world_object {

	class health_pickup : public pickup {
		float heal_amount;
	public:
		health_pickup(math::vec2 pos, float z, assets::texture_id tex,
		              float amount, entities::player& p,
		              geometry::map_data& md,
		              util::indexed_storage<geometry::subsector>::id_t sub_id,
		              float radius = 20.0f);
		void on_pickup(entities::player& p) override;
	};

}
