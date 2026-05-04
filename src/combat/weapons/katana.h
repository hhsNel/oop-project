#pragma once
#include "weapon.h"

namespace engine::combat {
	class katana : public weapon {
	public:
		int swing_count;

		explicit katana(float rate = 1.5f, float dmg = 50.0f);
		bool can_fire() const override;
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
