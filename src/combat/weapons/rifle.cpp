#include "rifle.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		rifle::rifle(geometry::map_data const* map)
			: weapon(std::make_unique<hitscan_firing_mode>(map), 20, 3.0f, 35.0f, 4) {}
	}
}
