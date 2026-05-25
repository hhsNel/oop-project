#include "smg.h"
#include "hitscan-firing-mode.h"

namespace combat {
	namespace weapons {
		smg::smg(geometry::map_data const* map)
			: weapon(std::make_unique<hitscan_firing_mode>(map), 30, 10.0f, 10.0f, 4) {}
	}
}
