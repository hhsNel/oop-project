#include "shotgun.h"

namespace combat {
	namespace weapons {
		shotgun::shotgun(std::unique_ptr<firing_mode> firing)
			: weapon(std::move(firing), 8, 1.0f, 15.0f, 4) {}

		void shotgun::fire(math::vec2 pos, float angle, std::span<engine::actor*> targets) {
			if (!can_fire()) return;
			float step        = spread / static_cast<float>(pellet_count - 1);
			float start_angle = angle - spread / 2.0f;
			for (int i = 0; i < pellet_count; ++i)
				ammo->spawn_bullet(pos, start_angle + static_cast<float>(i) * step, damage, targets);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
		}
	}
}
