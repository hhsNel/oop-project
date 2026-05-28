#include "pistol.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		pistol::pistol(geometry::map_data const& map, engine::world const& world,
		               audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 8, 2.0f, 25.0f, 5,
			         mix, am, 1, 2, 1.0f) {}
	}
}
