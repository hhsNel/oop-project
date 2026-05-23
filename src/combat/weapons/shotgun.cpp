#include "shotgun.h"

namespace combat {
	namespace weapons {
		shotgun::shotgun(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 8, 1.0f, 15.0f) {}

		void shotgun::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			float step        = spread / static_cast<float>(pellet_count - 1);
			float start_angle = angle - spread / 2.0f;
			for (int i = 0; i < pellet_count; ++i)
				ammo->spawn_bullet(pos, start_angle + static_cast<float>(i) * step, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
		}
	}
}
