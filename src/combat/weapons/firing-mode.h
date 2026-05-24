#pragma once

#include <span>
#include "math/vec2.h"

namespace engine { class actor; }

namespace combat {
	namespace weapons {
		/*
		Tryb strzelania, mowi co sie stanie po wystrzeleniu broni
		*/
		class firing_mode {
		public:
			virtual void spawn_bullet(math::vec2 pos, float angle, float damage,
			                          std::span<engine::actor*> targets = {}) = 0;
			virtual ~firing_mode() = default;
		};
	}
}
