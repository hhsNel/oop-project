#pragma once

#include "engine/renderable-entity.h"
#include "geometry/map-data.h"
#include "math/vec2.h"

namespace entities { class player; }

namespace world_object {

	class pickup : public engine::renderable_entity {
		float pickup_radius;
		entities::player& player_ref;
		geometry::map_data& map_ref;
		util::indexed_storage<geometry::subsector>::id_t subsector_id;

	public:
		pickup(math::vec2 pos, float z, assets::texture_id tex,
		       entities::player& p, geometry::map_data& md,
		       util::indexed_storage<geometry::subsector>::id_t sub_id,
		       float radius = 20.0f);

		void update(float dt) override;

		virtual void on_pickup(entities::player& p) = 0;
		virtual ~pickup() = default;
	};

}
