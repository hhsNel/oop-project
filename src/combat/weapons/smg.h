#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class smg : public weapon {
		public:
			smg(std::unique_ptr<firing_mode> ammo_type, int mag_size = 30, int max = 30, float rate = 10.0f, float dmg = 10.0f);
		};
	}
}
