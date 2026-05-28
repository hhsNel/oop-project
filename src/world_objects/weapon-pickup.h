#pragma once

#include <memory>
#include "pickup.h"

namespace combat { namespace weapons { class weapon; } }

namespace world_object {

	class weapon_pickup : public pickup {
		std::unique_ptr<combat::weapons::weapon> provided_weapon;
		int slot;
	public:
		weapon_pickup(math::vec2 pos, float z, assets::texture_id tex,
		              std::unique_ptr<combat::weapons::weapon> w, int weapon_slot,
		              entities::player& p, geometry::map_data& md,
		              util::indexed_storage<geometry::subsector>::id_t sub_id,
		              float radius = 25.0f, float scale = 0.4f);
		void on_pickup(entities::player& p) override;
	};

}
