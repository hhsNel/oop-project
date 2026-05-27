#include "rifle.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		rifle::rifle(geometry::map_data const& map, engine::world const& world,
		             audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 20, 3.0f, 35.0f, 4,
			         mix, am, 5, 6) {}
	}
}
