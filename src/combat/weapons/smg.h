#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class smg : public weapon {
		public:
			explicit smg(geometry::map_data const& map,
			             engine::world const& world,
			             audio::audio_mixer& mix,
			             assets::asset_manager const& am);
		};
	}
}
