#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class rifle : public weapon {
		public:
			explicit rifle(std::unique_ptr<firing_mode> firing);
		};
	}
}
