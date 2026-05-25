#include "pistol.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		pistol::pistol(geometry::map_data const* map)
			: weapon(std::make_unique<hitscan_firing_mode>(map), 8, 2.0f, 25.0f, 5) {}
	}
}
