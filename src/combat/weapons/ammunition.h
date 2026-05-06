
#pragma once
#define AMMUNITION_H

#include <memory>

#include "math/vec2.h"

namespace combat::weapons
{
	class ammunition {
	public:
		virtual void spawn_bullet(math::vec2 const pos, float const angle, float const damage) = 0;

		// Called every frame via weapon::update(); override for projectile-based ammo.
		virtual void update(float /*dt*/) {}

		virtual ~ammunition() = default;
	};
}

