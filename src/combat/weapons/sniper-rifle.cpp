#include "sniper-rifle.h"

namespace combat {
	namespace weapons {
		sniper_rifle::sniper_rifle(std::unique_ptr<firing_mode> firing)
			: weapon(ammo_type::sniper, std::move(firing), 5, 0.5f, 100.0f, 3) {}
	}
}
