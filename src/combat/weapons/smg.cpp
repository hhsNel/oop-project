#include "smg.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		smg::smg(geometry::map_data const& map, engine::world const& world,
		         audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 30, 10.0f, 10.0f, 4,
			         mix, am, 3, 4, 1.5f) {}
	}
}
