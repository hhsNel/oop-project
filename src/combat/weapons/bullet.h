#pragma once
#include "ammunition.h"
#include <vector>

namespace engine::combat {
	// Records spawned bullets for testing; replace body of spawn_bullet when world exists.
	class bullet_ammunition : public ammunition {
	public:
		struct bullet_record { math::vec2 pos; float angle; float damage; };
		std::vector<bullet_record> spawned;

		void spawn_bullet(math::vec2 pos, float angle, float damage) override;
	};
}
