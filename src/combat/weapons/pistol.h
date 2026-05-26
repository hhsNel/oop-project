#pragma once
#include "weapon.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace combat {
	namespace weapons {
		class pistol : public weapon {
		public:
			explicit pistol(geometry::map_data const& map,
			                engine::world const& world);
		};
	}
}
