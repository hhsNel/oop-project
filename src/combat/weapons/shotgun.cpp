#include "shotgun.h"
#include "hitscan-firing-mode.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"

namespace combat {
	namespace weapons {
		shotgun::shotgun(geometry::map_data const& map, engine::world const& world,
		                 audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 8, 1.0f, 15.0f, 4,
			         mix, am, 7, 8) {}

		void shotgun::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			float step        = spread / static_cast<float>(pellet_count - 1);
			float start_angle = angle - spread / 2.0f;
			for (int i = 0; i < pellet_count; ++i)
				ammo->spawn_bullet(pos, start_angle + static_cast<float>(i) * step, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
			if (fire_sound_id >= 0) mixer.play(assets.audio_clip_by_id(fire_sound_id));
		}
	}
}
