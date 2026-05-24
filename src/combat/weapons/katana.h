#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			explicit katana(std::unique_ptr<firing_mode> firing = nullptr);

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle, std::span<engine::actor*> targets = {}) override;
			void reload() override;
		};
	}
}
