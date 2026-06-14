#pragma once

#include "pickup.h"

namespace world_object {

	class armor_pickup : public pickup {
		float armor_amount;
	public:
		/* constructor */
		armor_pickup(math::vec2 pos, float z, assets::texture_id tex,
		             float amount, entities::player& p,
		             geometry::map_data& md,
		             util::indexed_storage<geometry::subsector>::id_t sub_id,
		             float radius = 20.0f);
		/* grant armor to the player */
		void on_pickup(entities::player& p) override;
	};

}
