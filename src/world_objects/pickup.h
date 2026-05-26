#pragma once

#include "engine/entity.h"
#include "engine/world.h"
#include "math/vec2.h"

namespace entities { class player; }

namespace world_object {

	class pickup : public engine::entity {
		math::vec2 position;
		float      pickup_radius;

		entities::player& player_ref;
		engine::world&    world_ref;
		util::indexed_storage<std::unique_ptr<engine::entity>>::id_t self_id;

	public:
		pickup(math::vec2 pos, entities::player& p, engine::world& w,
		       util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
		       float radius = 20.0f);

		void update(float dt) override;

		bool in_range(math::vec2 player_pos) const;

		virtual void on_pickup(entities::player& p) = 0;
		virtual ~pickup() = default;
	};

}
