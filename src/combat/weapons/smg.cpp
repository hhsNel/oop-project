#include "smg.h"

namespace combat {
	namespace weapons {
		smg::smg(std::unique_ptr<firing_mode> firing)
			: weapon(std::move(firing), 30, 10.0f, 10.0f, 4) {}
	}
}
