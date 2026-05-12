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

		/*
		Inicjalizacja health_system przez konstruktor zamiast ustawiania pol przez componentized.
		take_true_damage/take_damage deleguja do metod domenowych health_system.
		*/
		actor(float hp, float shield, float move_speed, faction this_team)
			: renderable_entity(), health(hp, hp, shield, shield),
			  angle(0.0f), movement_speed(move_speed), team(this_team)
		{}

		virtual void take_damage(float const dmg) { health.apply_damage(dmg); }
		virtual void take_true_damage(float const dmg) { health.apply_true_damage(dmg); }
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

