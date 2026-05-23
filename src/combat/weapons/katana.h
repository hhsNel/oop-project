#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana final : public weapon {
		public:
			katana();
			explicit katana(std::unique_ptr<firing_mode> firing);

			[[nodiscard]] bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
