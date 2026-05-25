#include "plasma-gun.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(geometry::map_data const* map, engine::world const* world,
		                       audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		                       assets::audio_clip const* reload_snd)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 10, 1.0f, 75.0f, 3,
			         mix, fire_snd, reload_snd) {}
	}
}
