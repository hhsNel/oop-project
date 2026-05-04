#pragma once
#include "weapon.h"

namespace engine::combat {
	class smg : public weapon {
	public:
		smg(std::unique_ptr<ammunition> ammo_type, int mag_size = 30, int max = 30, float rate = 10.0f, float dmg = 10.0f);
		void fire(math::vec2 pos, float angle) override;
		void reload() override;
	};
}
