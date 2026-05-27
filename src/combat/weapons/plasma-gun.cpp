#include "plasma-gun.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(geometry::map_data const& map, engine::world const& world,
		                       audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 10, 1.0f, 75.0f, 3,
			         mix, am, 11, 12) {}
	}
}
