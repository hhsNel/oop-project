#pragma once
#include "firing-mode.h"
#include "util/componentized.h"
#include <vector>

namespace combat {
	namespace weapons {
		// Testowy tryb strzelania — zapisuje kazdy strzal do wektora.
		// Uzywany w testach do weryfikacji pozycji, kata i obrazen.
		class recorded_firing_mode : public firing_mode, public util::componentized<recorded_firing_mode> {
			friend class util::componentized<recorded_firing_mode>;
		public:
			struct shot_record { math::vec2 pos; float angle; float damage; };
		private:
			[[=util::component_field{}]] std::vector<shot_record> spawned;

		public:
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;
		};
	}
}
