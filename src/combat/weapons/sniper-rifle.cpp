#include "sniper-rifle.h"

namespace combat {
	namespace weapons {
		sniper_rifle::sniper_rifle(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 5, 0.5f, 100.0f) {}
	}
}
