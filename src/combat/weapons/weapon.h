#pragma once

#include <memory>

#include "math/vec2.h"
#include "firing-mode.h"
#include "assets/ids.h"

namespace audio { class audio_mixer; }
namespace assets { class asset_manager; }

namespace game { class game; }

namespace game { class game; }

namespace combat
{
	namespace weapons {
		class weapon {
			friend class game::game;
		protected:
			std::unique_ptr<firing_mode> ammo;
			int ammo_count;
			int max_ammo;
			int reserve_mags;
			float fire_rate;
			float last_shot_time;
			float damage;
			audio::audio_mixer& mixer;
			assets::asset_manager const& assets;
			assets::audio_clip_id fire_sound_id;
			assets::audio_clip_id reload_sound_id;
			float reload_duration;
			float reload_timer;
			bool reloading;

			void finish_reload();
		public:
			virtual bool can_fire() const;
			void tick(float dt);
			void resupply(int amount);

			virtual void fire(math::vec2 pos, float angle);
			virtual void reload();
			virtual ~weapon() = default;

		protected:
			weapon(std::unique_ptr<firing_mode> firing, int max, float rate, float dmg,
			       int reserve,
			       audio::audio_mixer& mix,
			       assets::asset_manager const& am,
			       assets::audio_clip_id fire_snd = -1,
			       assets::audio_clip_id reload_snd = -1,
			       float reload_dur = 1.5f);
		};
	}
}
