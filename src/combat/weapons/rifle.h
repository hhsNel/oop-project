#pragma once
#include "weapon.h"

namespace engine::combat {
	class rifle : public weapon {
	public:
		rifle(std::unique_ptr<ammunition> ammo_type, int mag_size = 20, int max = 20, float rate = 3.0f, float dmg = 35.0f);
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
