#pragma once

#include "engine/renderable-entity.h"
#include "math/vec2.h"
#include "util/indexed-storage.h"
#include "geometry/subsector.h"

namespace entities { class player; }
namespace geometry { class map_data; }

namespace world_object {

	class pickup : public engine::renderable_entity {
		/* distance within which the player collects it */
		float pickup_radius;
		/* player that can collect this pickup */
		entities::player& player_ref;
		/* map the pickup lives in */
		geometry::map_data& map_ref;
		/* subsector the pickup is located in */
		util::indexed_storage<geometry::subsector>::id_t subsector_id;
		/* whether the pickup has already been collected */
		bool consumed = false;

	public:
		/* constructor */
		pickup(math::vec2 pos, float z, assets::texture_id tex,
		       entities::player& p, geometry::map_data& md,
		       util::indexed_storage<geometry::subsector>::id_t sub_id,
		       float radius = 20.0f, float scale = 1.0f);

		/* entity tick, consumes the pickup when the player is in range */
		void update(float dt) override;
		/* whether the pickup has already been collected */
		bool is_consumed() const;

		/* apply the pickup's effect to the player */
		virtual void on_pickup(entities::player& p) = 0;
		/* virtual destructor */
		virtual ~pickup() = default;
	};

}
