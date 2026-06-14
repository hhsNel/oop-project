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
			/* firing behaviour (hitscan or projectile) */
			std::unique_ptr<firing_mode> ammo;
			/* rounds left in the magazine */
			[[=util::component_field{}]] int ammo_count;
			/* magazine capacity */
			int max_ammo;
			/* spare magazines held in reserve */
			[[=util::component_field{}]] int reserve_mags;
			/* shots per second */
			float fire_rate;
			/* cooldown remaining until the next shot */
			float last_shot_time;
			/* damage dealt per shot */
			float damage;
			/* audio mixer used to play weapon sounds */
			audio::audio_mixer& mixer;
			/* asset manager for looking up audio clips */
			assets::asset_manager const& assets;
			/* sound played when firing */
			assets::audio_clip_id fire_sound_id;
			/* sound played when reloading */
			assets::audio_clip_id reload_sound_id;
			/* time a full reload takes */
			[[=util::component_field{}]] float reload_duration;
			/* time elapsed in the current reload */
			[[=util::component_field{}]] float reload_timer;
			/* whether a reload is in progress */
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
