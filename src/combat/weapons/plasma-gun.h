#pragma once
#include "weapon.h"

namespace engine::combat {
	class plasma_gun : public weapon {
	public:
		plasma_gun(std::unique_ptr<ammunition> ammo_type, int mag_size = 10, int max = 10, float rate = 1.0f, float dmg = 75.0f);
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
