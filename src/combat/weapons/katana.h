#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			katana(geometry::map_data const& map, engine::world const& world,
			       audio::audio_mixer& mix, assets::asset_manager const& am);

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
