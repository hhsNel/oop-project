#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class smg : public weapon {
		public:
			explicit smg(std::unique_ptr<firing_mode> ammo_type);
		};
	}
}
