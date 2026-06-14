#pragma once

#include <memory>

#include "math/vec2.h"
#include "firing-mode.h"
#include "assets/ids.h"
#include "util/componentized.h"

namespace audio { class audio_mixer; }
namespace assets { class asset_manager; }

namespace combat
{
	namespace weapons {
		class weapon : public util::componentized<weapon> {
			friend class util::componentized<weapon>;
		protected:
			std::unique_ptr<firing_mode> ammo;
			[[=util::component_field{}]] int ammo_count;
			int max_ammo;
			[[=util::component_field{}]] int reserve_mags;
			float fire_rate;
			float last_shot_time;
			float damage;
			audio::audio_mixer& mixer;
			assets::asset_manager const& assets;
			assets::audio_clip_id fire_sound_id;
			assets::audio_clip_id reload_sound_id;
			[[=util::component_field{}]] float reload_duration;
			[[=util::component_field{}]] float reload_timer;
			[[=util::component_field{}]] bool reloading;

			/* refill the magazine at the end of a reload */
			void finish_reload();
		public:
			/* whether the weapon can currently fire */
			virtual bool can_fire() const;
			/* advance cooldown and reload timers */
			void tick(float dt);
			/* add spare magazines to the reserve */
			void resupply(int amount);

			/* fire the weapon from a position at an angle */
			virtual void fire(math::vec2 pos, float angle);
			/* begin reloading from the reserve */
			virtual void reload();
			/* virtual destructor */
			virtual ~weapon() = default;

		protected:
			/* constructor */
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
