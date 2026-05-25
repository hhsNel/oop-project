#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			katana();

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
