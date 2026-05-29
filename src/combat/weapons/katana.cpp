#include "katana.h"
#include "hitscan-firing-mode.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"

namespace combat {
	namespace weapons {
		katana::katana(geometry::map_data const& map, engine::world const& world,
		               audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world, 60.0f, 32.0f), 0, 1.5f, 50.0f, 0, mix, am, 13) {}

		bool katana::can_fire() const {
			return last_shot_time <= 0.0f;
		}

		void katana::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			if (ammo) ammo->spawn_bullet(pos, angle, damage);
			last_shot_time = 1.0f / fire_rate;
			if (fire_sound_id >= 0) mixer.play(assets.audio_clip_by_id(fire_sound_id));
		}

		void katana::reload() {}
	}
}
