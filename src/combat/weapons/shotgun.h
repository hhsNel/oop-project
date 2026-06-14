#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class shotgun : public weapon {
		public:
			/* number of pellets fired per shot */
			static constexpr int pellet_count = 8;
			/* angular spread between pellets, in radians */
			static constexpr float spread = 0.2618f;

			/* constructor */
			explicit shotgun(geometry::map_data const& map,
			                 engine::world const& world,
			                 audio::audio_mixer& mix,
			                 assets::asset_manager const& am);
			/* fire a spread of pellets in one shot */
			void fire(math::vec2 pos, float angle) override;
		};
	}
}
