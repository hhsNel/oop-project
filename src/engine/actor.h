#pragma once
#define ACTOR_H

#include "renderable-entity.h"
#include "systems/systems.h"

namespace engine {

	enum class faction {
		player,
		enemy,
		neutral
	};

	class actor : virtual public renderable_entity {
	protected:
		systems::health_system health;
	public:
		float      angle;
		float      movement_speed;
		faction    team;

		actor(float hp, float shield, float move_speed, faction this_team);

		virtual void take_damage(float const dmg);
		virtual void take_true_damage(float const dmg);
		virtual void heal(float const amount);
		virtual void add_shield(float const amount);
		virtual void add_effect(std::unique_ptr<combat::status_effect> effect);

		bool is_dead() const { return health.is_dead(); }

		virtual void update(float dt) override;
	};
}

