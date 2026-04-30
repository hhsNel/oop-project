#pragma once
#define WEAPON_H

#include <memory>

#include "math/vec2.h"
#include "ammunition.h"

namespace engine::combat
{
	class weapon {
	public:
		unsigned int weapon_id;
		std::unique_ptr<ammunition> ammo;
		int ammo_count;
		int max_ammo;
		float fire_rate;
		float last_shot_time;

		bool can_fire() const {return ammo_count>0;}

		virtual void fire(math::vec2 const pos, float const angle) = 0;
		virtual void reload() = 0;
		virtual ~weapon() = default;
	};
}

