#include "sniper-rifle.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		sniper_rifle::sniper_rifle(geometry::map_data const& map, engine::world const& world,
		                           audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		                           assets::audio_clip const* reload_snd)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 5, 0.5f, 100.0f, 3,
			         mix, fire_snd, reload_snd) {}
	}
}
