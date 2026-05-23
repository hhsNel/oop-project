#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class smg final : public weapon {
		public:
			explicit smg(std::unique_ptr<firing_mode> firing);
		};
	}
}
