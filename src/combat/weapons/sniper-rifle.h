#pragma once
#include "weapon.h"

namespace combat::weapons {
	class sniper_rifle : public weapon {
	public:
		sniper_rifle(std::unique_ptr<ammunition> ammo_type, int mag_size = 5, int max = 5, float rate = 0.5f, float dmg = 100.0f);
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
