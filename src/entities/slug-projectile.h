#pragma once

#include "engine/projectile.h"

namespace entities {

	class slug_projectile : public engine::projectile {
	public:
		/* inherit the base projectile constructors */
		using projectile::projectile;
	};

}
