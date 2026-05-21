#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		// Shotgun nadpisuje fire() — strzela wieloma pelletami z rozrzutem.
		class shotgun : public weapon {
		public:
			static constexpr int   pellet_count = 8;
			static constexpr float spread       = 0.2618f; // ~15 degrees total

			shotgun(std::unique_ptr<firing_mode> ammo_type, int mag_size = 8, int max = 8, float rate = 1.0f, float dmg = 15.0f);
			void fire(math::vec2 pos, float angle) override;
		};
	}
}
