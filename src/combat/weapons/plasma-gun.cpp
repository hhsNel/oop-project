#include "plasma-gun.h"
#include "projectile-firing-mode.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(geometry::map_data& map, engine::world& world,
		                       audio::audio_mixer& mix, assets::asset_manager const& am)
			: weapon(std::make_unique<projectile_firing_mode>(
				world, map, engine::faction::player, 30, 400.0f, 5.0f, 1.0f),
				10, 1.0f, 200.0f, 3, mix, am, 11, 12, 5.0f) {}
	}
}
