#pragma once

#include <memory>

#include "math/vec2.h"
#include "firing-mode.h"

namespace audio { class audio_mixer; }
namespace assets { class audio_clip; }

namespace combat
{
	namespace weapons {
		class weapon {
		protected:
			std::unique_ptr<firing_mode> ammo;
			int ammo_count;
			int max_ammo;
			int reserve_mags;
			float fire_rate;
			float last_shot_time;
			float damage;
			audio::audio_mixer* mixer;
			assets::audio_clip const* fire_sound;
			assets::audio_clip const* reload_sound;
		public:
			virtual bool can_fire() const;
			void tick(float dt);
			void resupply(int amount);

			virtual void fire(math::vec2 pos, float angle);
			virtual void reload();
			virtual ~weapon() = default;

		protected:
			weapon(std::unique_ptr<firing_mode> firing, int max, float rate, float dmg,
			       int reserve = 0,
			       audio::audio_mixer* mix = nullptr,
			       assets::audio_clip const* fire_snd = nullptr,
			       assets::audio_clip const* reload_snd = nullptr);
		};
	}
}
