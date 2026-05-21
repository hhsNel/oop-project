#pragma once
#include "firing-mode.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Testowy tryb strzelania — zapisuje kazdy strzal do wektora.
		// Uzywany w testach do weryfikacji pozycji, kata i obrazen.
		class recorded_firing_mode : public firing_mode {
		public:
			struct shot_record { math::vec2 pos; float angle; float damage; };
		private:
			std::vector<shot_record> spawned;

		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;

			size_t shot_count() const { return spawned.size(); }
			shot_record const& shot_at(size_t i) const { return spawned[i]; }
			std::vector<shot_record> const& all_shots() const { return spawned; }
		};
	}
}
