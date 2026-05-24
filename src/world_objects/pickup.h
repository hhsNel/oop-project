#pragma once

#include "engine/entity.h"
#include "math/vec2.h"

namespace entities { class player; }

namespace world_object {

	/*
	Abstract base for all collectible world items.
	Each frame, the game loop checks in_range(player.position) and calls
	on_pickup(player) when the player is close enough.
	*/
	class pickup : public engine::entity {
		math::vec2 position;
		float      pickup_radius;
	protected:
		bool collected;
	public:
		pickup(math::vec2 pos, float radius = 20.0f);

		void update(float dt) override;

		bool in_range(math::vec2 player_pos) const;

		virtual void on_pickup(entities::player& p) = 0;
		virtual ~pickup() = default;
	};

}
