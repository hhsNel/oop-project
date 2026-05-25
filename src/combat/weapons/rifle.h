#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class rifle : public weapon {
		public:
			explicit rifle(geometry::map_data const* map = nullptr,
			               engine::world const* world = nullptr);
		};
	}
}
