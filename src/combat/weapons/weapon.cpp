#include "weapon.h"
#include "audio/audio-mixer.h"
#include "assets/audio-clip.h"
#include <algorithm>

namespace combat {
	namespace weapons {

		weapon::weapon(std::unique_ptr<firing_mode> firing, int max, float rate, float dmg, int reserve,
		               audio::audio_mixer* mix, assets::audio_clip const* fire_snd,
		               assets::audio_clip const* reload_snd)
			: ammo(std::move(firing)), ammo_count(max),
			  max_ammo(max), reserve_mags(reserve), fire_rate(rate), last_shot_time(0.0f), damage(dmg),
			  mixer(mix), fire_sound(fire_snd), reload_sound(reload_snd) {}

		bool weapon::can_fire() const {
			return ammo_count > 0 && last_shot_time <= 0.0f;
		}

		void weapon::tick(float dt) {
			last_shot_time = std::max(0.0f, last_shot_time - dt);
		}

		void weapon::resupply(int mags) {
			reserve_mags += mags;
		}

		void weapon::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			ammo->spawn_bullet(pos, angle, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
			if (mixer && fire_sound) mixer->play(*fire_sound);
		}

		void weapon::reload() {
			if (reserve_mags <= 0) return;
			if (mixer && reload_sound) mixer->play(*reload_sound);
			--reserve_mags;
			ammo_count = max_ammo;
		}
	}
}
