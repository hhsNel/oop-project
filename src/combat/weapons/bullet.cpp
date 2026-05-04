#include "bullet.h"

namespace engine::combat {
	void bullet_ammunition::spawn_bullet(math::vec2 pos, float angle, float damage) {
		spawned.push_back({pos, angle, damage});
	}
}
