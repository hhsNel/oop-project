#include "katana.h"
#include "audio/audio-mixer.h"

namespace combat {
	namespace weapons {
		katana::katana(audio::audio_mixer* mix, assets::audio_clip const* fire_snd)
			: weapon(nullptr, 0, 1.5f, 50.0f, 0, mix, fire_snd, nullptr) {}

		bool katana::can_fire() const {
			return last_shot_time <= 0.0f;
		}

		void katana::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			if (ammo) ammo->spawn_bullet(pos, angle, damage);
			last_shot_time = 1.0f / fire_rate;
			if (mixer && fire_sound) mixer->play(*fire_sound);
		}

		void katana::reload() {}
	}
}
