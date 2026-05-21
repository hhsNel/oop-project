#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class plasma_gun : public weapon {
		public:
			plasma_gun(std::unique_ptr<firing_mode> ammo_type, int mag_size = 10, int max = 10, float rate = 1.0f, float dmg = 75.0f);
		};
	}
}
