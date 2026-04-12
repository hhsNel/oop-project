#pragma once
#define ACTOR_H

#include "entity.h"

namespace engine {
	class actor : virtual public entity {
		unsigned int hp, shield;
	public:
		actor(unsigned int const hp = 0, unsigned int const shield = 0);

		virtual void take_damage(unsigned int const dmg);
		virtual void heal(unsigned int const amount);
		virtual void add_shield(unsigned int const amount);
	};
}

