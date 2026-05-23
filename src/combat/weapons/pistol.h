#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class pistol : public weapon {
		public:
			explicit pistol(std::unique_ptr<firing_mode> firing);
		};
	}
}
