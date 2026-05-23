#include "plasma-gun.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 10, 1.0f, 75.0f) {}
	}
}
