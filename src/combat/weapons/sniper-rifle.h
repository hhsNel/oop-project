#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class sniper_rifle : public weapon {
		public:
			explicit sniper_rifle(std::unique_ptr<firing_mode> ammo_type);
		};
	}
}
