#pragma once

#include "math/vec2.h"

namespace combat {
	namespace weapons {
		class firing_mode {
		public:
			virtual void spawn_bullet(math::vec2 pos, float angle, float damage) = 0;
			virtual ~firing_mode() = default;
		};
	}
}
