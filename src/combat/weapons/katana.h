#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			/* constructor */
			katana(geometry::map_data const& map, engine::world const& world,
			       audio::audio_mixer& mix, assets::asset_manager const& am);

			/* ready to swing once the cooldown elapses (needs no ammo) */
			bool can_fire() const override;
			/* perform a short-range melee swing */
			void fire(math::vec2 pos, float angle) override;
			/* no-op, the katana never reloads */
			void reload() override;
		};
	}
}
