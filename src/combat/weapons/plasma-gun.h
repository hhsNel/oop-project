#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class plasma_gun final : public weapon {
		public:
			explicit plasma_gun(std::unique_ptr<firing_mode> firing);
		};
	}
}
