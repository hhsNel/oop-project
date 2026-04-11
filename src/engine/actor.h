#pragma once
#define ACTOR_H

#include "entity.h"

namespace engine {
	class actor : virtual public entity {
		unsigned int hp, shield;
	public:
		actor(unsigned int hp = 0, shield = 0);

		virtual void take_damage(unsigned int dmg);
		virtual void heal(unsigned int amount);
		virtual void add_shield(unsigned int amount);
	};
}

