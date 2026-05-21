#include "smg.h"

namespace combat {
	namespace weapons {
		smg::smg(std::unique_ptr<firing_mode> ammo_type, int mag_size, int max, float rate, float dmg)
			: weapon(0, std::move(ammo_type), mag_size, max, rate, dmg) {}
	}
}
