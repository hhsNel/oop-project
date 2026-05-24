#include "pistol.h"

namespace combat {
	namespace weapons {
		pistol::pistol(std::unique_ptr<firing_mode> firing)
			: weapon(std::move(firing), 8, 2.0f, 25.0f, 5) {}
	}
}
