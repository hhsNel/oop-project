#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class rifle : public weapon {
		public:
			rifle(std::unique_ptr<firing_mode> ammo_type, int mag_size = 20, int max = 20, float rate = 3.0f, float dmg = 35.0f);
		};
	}
}
