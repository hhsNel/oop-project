#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana : public weapon, public util::componentized<katana> {
			[[=util::component_field{}]] int swing_count;

			friend class util::componentized<katana>;
		public:
			using util::componentized<katana>::operator();
			using util::componentized<weapon>::operator();

			explicit katana(float rate = 1.5f, float dmg = 50.0f);
			// ammo_type should be a hitscan_ammunition with short range (~32 units) for melee
			katana(std::unique_ptr<ammunition> ammo_type, float rate = 1.5f, float dmg = 50.0f);
			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
