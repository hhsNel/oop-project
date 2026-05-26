#include "rifle.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		rifle::rifle(geometry::map_data const& map, engine::world const& world,
		             audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		             assets::audio_clip const* reload_snd)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 20, 3.0f, 35.0f, 4,
			         mix, fire_snd, reload_snd) {}
	}
}
