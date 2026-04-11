#pragma once
#define WEAPON_H

#include <memory>

#include "math/fxpt.h"
#include "math/vec2.h"
#include "ammunition.h"

namespace engine::combat
{
	class weapon {
	public:
		std::unique_ptr<ammunition> ammo;
		math::fxpt fire_rate;
		math::fxpt last_shot_time;
		
		virtual void fire(math::vec2 const pos, math::fxpt const angle) = 0;
		virtual void reload() = 0;
		virtual ~weapon() = default;
	};
}

