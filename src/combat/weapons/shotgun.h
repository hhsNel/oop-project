#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		// Shotgun nadpisuje fire() — strzela wieloma pelletami z rozrzutem.
		class shotgun : public weapon {
		public:
			static constexpr int   pellet_count = 8;
			static constexpr float spread       = 0.2618f; // ~15 degrees total

			explicit shotgun(std::unique_ptr<firing_mode> ammo_type);
			void fire(math::vec2 pos, float angle) override;
		};
	}
}
