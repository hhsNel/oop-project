#pragma once

#include "renderable-entity.h"
#include "systems/systems.h"

namespace engine {

	enum class faction {
		player,
		enemy,
		neutral
	};

	class actor : public renderable_entity {
	protected:
		systems::health_system health;
	public:
		float	  angle;
		float	  movement_speed;
		faction	team;

		actor(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float const hp, float const shield, float const move_speed, faction const this_team)
			: renderable_entity(p, z, tex, is), angle(0.0f), movement_speed(move_speed), team(this_team)
		{
			health.max_hp = hp;
			health.current_hp = hp;
			health.armor = shield;
			health.max_armor = shield;
		}

		virtual void take_damage(float const dmg);
		virtual void take_true_damage(float const dmg);
		virtual void heal(float const amount);
		virtual void add_shield(float const amount);
		virtual void add_effect(std::unique_ptr<combat::status_effect> effect);

		[[nodiscard]] bool is_dead() const { return health.is_dead(); }

		virtual void update(float dt) override;
	};
}

