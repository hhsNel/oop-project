#include "rifle.h"

namespace combat {
	namespace weapons {
		rifle::rifle(std::unique_ptr<firing_mode> firing)
			: weapon(std::move(firing), 20, 3.0f, 35.0f, 4) {}
	}
}
