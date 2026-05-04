#pragma once

#include "engine/projectile.h"

namespace engine::entities {

	class plasma_projectile : public projectile {
	public:
		float speed;
		int damage;
	};

	class grenade_projectile : public projectile {
	public:
		float fuse_time;
		float explosion_radius;
	};

	class slug_projectile : public projectile {};

}
