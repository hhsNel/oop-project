#include "bullet.h"

namespace combat {
	namespace weapons {
		void bullet_ammunition::spawn_bullet(math::vec2 pos, float angle, float damage) {
			spawned.push_back({pos, angle, damage});
		}
	}
}
