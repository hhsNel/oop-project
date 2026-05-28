#include "weapon.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"
#include <algorithm>

namespace combat {
	namespace weapons {

		weapon::weapon(std::unique_ptr<firing_mode> firing, int max, float rate, float dmg, int reserve,
		               audio::audio_mixer& mix, assets::asset_manager const& am,
		               assets::audio_clip_id fire_snd, assets::audio_clip_id reload_snd,
		               float reload_dur)
			: ammo(std::move(firing)), ammo_count(max),
			  max_ammo(max), reserve_mags(reserve), fire_rate(rate), last_shot_time(0.0f), damage(dmg),
			  mixer(mix), assets(am), fire_sound_id(fire_snd), reload_sound_id(reload_snd),
			  reload_duration(reload_dur), reload_timer(0.0f), reloading(false) {}

		bool weapon::can_fire() const {
			return ammo_count > 0 && last_shot_time <= 0.0f && !reloading;
		}

		void weapon::tick(float dt) {
			last_shot_time = std::max(0.0f, last_shot_time - dt);
			if (reloading) {
				reload_timer += dt;
				if (reload_timer >= reload_duration)
					finish_reload();
			}
		}

		void weapon::resupply(int mags) {
			reserve_mags += mags;
		}

		void weapon::fire(math::vec2 pos, float angle) {
			if (!can_fire()) return;
			ammo->spawn_bullet(pos, angle, damage);
			--ammo_count;
			last_shot_time = 1.0f / fire_rate;
			if (fire_sound_id >= 0) mixer.play(assets.audio_clip_by_id(fire_sound_id));
		}

		void weapon::reload() {
			if (reloading) return;
			if (reserve_mags <= 0) return;
			if (ammo_count == max_ammo) return;
			reloading = true;
			reload_timer = 0.0f;
			if (reload_sound_id >= 0) mixer.play(assets.audio_clip_by_id(reload_sound_id));
		}

		void weapon::finish_reload() {
			--reserve_mags;
			ammo_count = max_ammo;
			reloading = false;
		}
	}
}
