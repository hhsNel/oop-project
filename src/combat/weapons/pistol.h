#pragma once
#include "weapon.h"

namespace engine::combat {
	class pistol : public weapon {
	public:
		pistol(std::unique_ptr<ammunition> ammo_type, int mag_size = 8, int max = 8, float rate = 2.0f, float dmg = 25.0f);
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
