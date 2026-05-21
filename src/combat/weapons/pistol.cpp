#include "pistol.h"

namespace combat {
	namespace weapons {
		pistol::pistol(std::unique_ptr<firing_mode> ammo_type, int mag_size, int max, float rate, float dmg)
			: weapon(0, std::move(ammo_type), mag_size, max, rate, dmg) {}
	}
}
