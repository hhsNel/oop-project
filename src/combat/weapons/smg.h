#pragma once
#include "weapon.h"

namespace geometry { class map_data; }

namespace combat {
	namespace weapons {
		class smg : public weapon {
		public:
			explicit smg(geometry::map_data const* map = nullptr);
		};
	}
}
