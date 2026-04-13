#pragma once
#define ACTOR_H

#include "entity.h"
#include "math/fxpt.h"
#include "systems/systems.h"


namespace engine {
	class actor : virtual public entity {
	protected:
		systems::health_system health;
	public:
		
		actor(math::fxpt hp, math::fxpt shield)
		{
			health.max_hp = hp;
			health.current_hp = hp;
			health.armor = shield;
		}

		virtual void take_damage(math::fxpt const dmg)
		{
			health.apply_damage(dmg, combat::status_type::none);
		}
		virtual void heal(math::fxpt const amount)
		{
			health.apply_heal(amount);
		}
		virtual void add_shield(math::fxpt const amount)
		{
			health.apply_shield(amount);
		}

		virtual void update(math::fxpt dt)
		{
			health.process_effects(dt);
		}

	};
}

