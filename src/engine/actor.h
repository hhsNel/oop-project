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

		actor(float hp, float shield, float move_speed, faction this_team)
			: renderable_entity(), angle(0.0f), movement_speed(move_speed), team(this_team)
		{
			health("max_hp"_f) = hp;
			health("current_hp"_f) = hp;
			health("armor"_f) = shield;
			health("max_armor"_f) = shield;
		}

		virtual void take_damage(float const dmg) { health.apply_damage(dmg); }
		virtual void take_true_damage(float const dmg) { health("current_hp"_f) = std::max(0.0f, health("current_hp"_f) - dmg); }
		virtual void heal(float const amount) { health.apply_heal(amount); }
		virtual void add_shield(float const amount) { health.apply_shield(amount); }

		virtual void add_effect(std::unique_ptr<combat::status_effect> effect)
		{
			health.add_effect(std::move(effect), *this);
		}

		bool is_dead() const { return health.is_dead(); }

		virtual void update(float dt) override
		{
			health.process_effects(dt, *this);
		}
	};
}

