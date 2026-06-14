#pragma once

#include "engine/projectile.h"

namespace entities {

	class plasma_projectile : public engine::projectile {
	public:
		/* inherit the base projectile constructors */
		using projectile::projectile;
	};

}
