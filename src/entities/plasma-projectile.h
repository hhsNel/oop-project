#pragma once

#include "engine/projectile.h"

namespace entities {

	class plasma_projectile : public projectile {
	public:
		float speed;
		int damage;
	};

}
