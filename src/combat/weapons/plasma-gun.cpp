#include "plasma-gun.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(geometry::map_data const* map, engine::world const* world)
			: weapon(std::make_unique<hitscan_firing_mode>(map, world), 10, 1.0f, 75.0f, 3) {}
	}
}
