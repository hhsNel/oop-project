#include "sniper-rifle.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		sniper_rifle::sniper_rifle(geometry::map_data const& map, engine::world const& world,
		                           audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 5, 0.5f, 100.0f, 3,
			         mix, am, 9, 10) {}
	}
}
