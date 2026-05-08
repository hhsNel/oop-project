#pragma once
#include "ammunition.h"
#include "util/componentized.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Records spawned bullets for testing; replace body of spawn_bullet when world exists.
		class bullet_ammunition : public ammunition, public util::componentized<bullet_ammunition> {
		public:
			struct bullet_record { math::vec2 pos; float angle; float damage; };
		private:
			[[=util::component_field{}]] std::vector<bullet_record> spawned;

			friend class util::componentized<bullet_ammunition>;
		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;
		};
	}
}
