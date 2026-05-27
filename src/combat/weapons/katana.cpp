#include "katana.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"

namespace combat {
	namespace weapons {
		katana::katana(audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(nullptr, 0, 1.5f, 50.0f, 0, mix, am, 13) {}

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
