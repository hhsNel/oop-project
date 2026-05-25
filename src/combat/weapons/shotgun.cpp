#include "shotgun.h"
#include "hitscan-firing-mode.h"
#include "audio/audio-mixer.h"

namespace combat {
	namespace weapons {
		shotgun::shotgun(geometry::map_data const* map, engine::world const* world,
		                 audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		                 assets::audio_clip const* reload_snd)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 8, 1.0f, 15.0f, 4,
			         mix, fire_snd, reload_snd) {}

		void shotgun::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			float step        = spread / static_cast<float>(pellet_count - 1);
			float start_angle = angle - spread / 2.0f;
			for (int i = 0; i < pellet_count; ++i)
				ammo->spawn_bullet(pos, start_angle + static_cast<float>(i) * step, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
			if (mixer && fire_sound) mixer->play(*fire_sound);
		}
	}
}
