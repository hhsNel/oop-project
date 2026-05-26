#pragma once

#include "engine/entity.h"
#include "engine/world.h"
#include "math/vec2.h"

namespace entities { class player; }

namespace world_object {

	class pickup : public engine::entity {
		math::vec2 position;
		float      pickup_radius;

		entities::player* player_ref = nullptr;
		engine::world*    world_ref  = nullptr;
		util::indexed_storage<std::unique_ptr<engine::entity>>::id_t self_id = 0;

	public:
		pickup(math::vec2 pos, float radius = 20.0f);

		void bind(entities::player* p, engine::world* w,
		          util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id);

		void update(float dt) override;

		bool in_range(math::vec2 player_pos) const;

		virtual void on_pickup(entities::player& p) = 0;
		virtual ~pickup() = default;
	};

}
