#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class pistol : public weapon {
		public:
			pistol(std::unique_ptr<firing_mode> ammo_type, int mag_size = 8, int max = 8, float rate = 2.0f, float dmg = 25.0f);
		};
	}
}
