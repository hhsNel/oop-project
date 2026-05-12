#pragma once
#include "ammunition.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Records spawned plasma shots; replace body when world/projectile system exists.
		// Usunieto componentized — dostep przez metody domenowe (shot_count, shot_at, all_shots).
		class plasma_ammunition : public ammunition {
		public:
			struct plasma_record { math::vec2 pos; float angle; float damage; };
		private:
			std::vector<plasma_record> spawned;

		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;

			size_t shot_count() const { return spawned.size(); }
			plasma_record const& shot_at(size_t i) const { return spawned[i]; }
			std::vector<plasma_record> const& all_shots() const { return spawned; }
		};
	}
}
