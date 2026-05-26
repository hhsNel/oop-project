#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class rifle : public weapon {
		public:
			explicit rifle(geometry::map_data const& map,
			               engine::world const& world,
			               audio::audio_mixer* mix = nullptr,
			               assets::audio_clip const* fire_snd = nullptr,
			               assets::audio_clip const* reload_snd = nullptr);
		};
	}
}
