#include "pistol.h"

namespace combat {
	namespace weapons {
		pistol::pistol(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 8, 2.0f, 25.0f, 5) {}
	}
}
