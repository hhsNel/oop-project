#include "recorded-firing-mode.h"

namespace combat {
	namespace weapons {
		void recorded_firing_mode::spawn_bullet(math::vec2 pos, float angle, float damage) {
			spawned.push_back({pos, angle, damage});
		}
	}
}
