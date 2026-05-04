#pragma once
#include "ammunition.h"
#include <vector>

namespace engine::combat {
	// Records spawned plasma shots; replace body when world/projectile system exists.
	class plasma_ammunition : public ammunition {
	public:
		struct plasma_record { math::vec2 pos; float angle; float damage; };
		std::vector<plasma_record> spawned;

		void spawn_bullet(math::vec2 pos, float angle, float damage) override;
	};
}
