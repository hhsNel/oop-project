#include "pistol.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		pistol::pistol(geometry::map_data const& map, engine::world const& world,
		               audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		               assets::audio_clip const* reload_snd)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 8, 2.0f, 25.0f, 5,
			         mix, fire_snd, reload_snd) {}
	}
}
