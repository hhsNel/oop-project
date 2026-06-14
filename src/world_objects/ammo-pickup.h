#pragma once

#include "pickup.h"
#include "entities/player.h"

namespace world_object {

	template<typename WeaponT>
	class ammo_pickup : public pickup {
		int amount;
	public:
		/* constructor */
		ammo_pickup(math::vec2 position, float z, assets::texture_id tex,
		            int amt, entities::player& pl,
		            geometry::map_data& md,
		            util::indexed_storage<geometry::subsector>::id_t sub_id,
		            float radius = 20.0f)
			: pickup(position, z, tex, pl, md, sub_id, radius), amount(amt) {}

		/* resupply ammo for the matching weapon type */
		void on_pickup(entities::player& p) override {
			p.resupply<WeaponT>(amount);
		}
	};

}
