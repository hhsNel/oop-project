#pragma once
#define ACTOR_H

#include "entity.h"
#include "systems/systems.h"
#include "math/vec2.h"


namespace engine {

	enum class faction {
		player,
		enemy,
		neutral
	};

	class actor : virtual public entity {
	protected:
		systems::health_system health;
	public:
		math::vec2 position;
		float      angle;
		float movement_speed;
		faction team;

		actor(float hp, float shield, float move_speed, faction this_team)
			: position(0.0f, 0.0f), angle(0.0f), movement_speed(move_speed), team(this_team)
		{
			health.max_hp = hp;
			health.current_hp = hp;
			health.armor = shield;
			health.max_armor =shield;
		}

		virtual void take_damage(float const dmg)
		{
			health.apply_damage(dmg);
		}
		virtual void take_true_damage(float const dmg)
		{
			health.current_hp = std::max(0.0f, health.current_hp - dmg);
		}
		virtual void heal(float const amount)
		{
			health.apply_heal(amount);
		}
		virtual void add_shield(float const amount)
		{
			health.apply_shield(amount);
		}

		virtual void add_effect(std::unique_ptr<combat::status_effect> effect)
		{
			health.add_effect(std::move(effect), *this);
		}

		bool is_dead() const { return health.is_dead(); }

		virtual void update(float dt)
		{
			health.process_effects(dt, *this);
		}

	};
}

