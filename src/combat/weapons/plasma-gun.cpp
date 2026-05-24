#include "plasma-gun.h"

namespace combat {
	namespace weapons {
		plasma_gun::plasma_gun(std::unique_ptr<firing_mode> firing)
			: weapon(std::move(firing), 10, 1.0f, 75.0f, 3) {}
	}
}
