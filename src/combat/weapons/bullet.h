#pragma once
#include "ammunition.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Records spawned bullets for testing; replace body of spawn_bullet when world exists.
		// Usunieto componentized — dostep do danych przez metody domenowe
		// (shot_count, shot_at, all_shots) zamiast eksportu wektora spawned.
		class bullet_ammunition : public ammunition {
		public:
			struct bullet_record { math::vec2 pos; float angle; float damage; };
		private:
			std::vector<bullet_record> spawned;

		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;

			size_t shot_count() const { return spawned.size(); }
			bullet_record const& shot_at(size_t i) const { return spawned[i]; }
			std::vector<bullet_record> const& all_shots() const { return spawned; }
		};
	}
}
