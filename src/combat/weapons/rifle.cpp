#include "rifle.h"

namespace combat {
	namespace weapons {
		rifle::rifle(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 20, 3.0f, 35.0f, 4) {}
	}
}
