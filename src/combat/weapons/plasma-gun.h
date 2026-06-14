#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class plasma_gun : public weapon {
		public:
			/* constructor */
			explicit plasma_gun(geometry::map_data& map,
			                    engine::world& world,
			                    audio::audio_mixer& mix,
			                    assets::asset_manager const& am);
		};
	}
}
