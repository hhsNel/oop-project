#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		// Shotgun nadpisuje fire() — strzela wieloma pelletami z rozrzutem.
		class shotgun : public weapon {
		public:
			static constexpr int   pellet_count = 8;
			static constexpr float spread       = 0.2618f; // ~15 degrees total

			explicit shotgun(geometry::map_data const& map,
			                 engine::world const& world,
			                 audio::audio_mixer& mix,
			                 assets::asset_manager const& am);
			void fire(math::vec2 pos, float angle) override;
		};
	}
}
