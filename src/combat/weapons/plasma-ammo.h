#pragma once
#include "ammunition.h"
#include "util/componentized.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Records spawned plasma shots; replace body when world/projectile system exists.
		class plasma_ammunition : public ammunition, public util::componentized<plasma_ammunition> {
		public:
			struct plasma_record { math::vec2 pos; float angle; float damage; };
		private:
			[[=util::component_field{}]] std::vector<plasma_record> spawned;

			friend class util::componentized<plasma_ammunition>;
		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;
		};
	}
}
