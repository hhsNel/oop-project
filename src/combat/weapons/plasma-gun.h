#pragma once
#include "weapon.h"

namespace geometry { class map_data; }

namespace combat {
	namespace weapons {
		class plasma_gun : public weapon {
		public:
			explicit plasma_gun(geometry::map_data const* map = nullptr);
		};
	}
}
