#pragma once
#include "weapon.h"

namespace geometry { class map_data; }

namespace combat {
	namespace weapons {
		class sniper_rifle : public weapon {
		public:
			explicit sniper_rifle(geometry::map_data const* map = nullptr);
		};
	}
}
