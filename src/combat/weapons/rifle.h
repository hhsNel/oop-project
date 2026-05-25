#pragma once
#include "weapon.h"

namespace geometry { class map_data; }

namespace combat {
	namespace weapons {
		class rifle : public weapon {
		public:
			explicit rifle(geometry::map_data const* map = nullptr);
		};
	}
}
