#include "smg.h"

namespace combat {
	namespace weapons {
		smg::smg(std::unique_ptr<firing_mode> ammo_type)
			: weapon(0, std::move(ammo_type), 30, 10.0f, 10.0f) {}
	}
}
