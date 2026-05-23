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

			katana();
			explicit katana(std::unique_ptr<firing_mode> ammo_type);

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
