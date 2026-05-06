#include "plasma-ammo.h"

namespace combat::weapons {
	void plasma_ammunition::spawn_bullet(math::vec2 pos, float angle, float damage) {
		spawned.push_back({pos, angle, damage});
	}
}
