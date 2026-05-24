#include "status-effect.h"

namespace combat {

	bool status_effect::tick(float dt) {
		duration -= dt;
		if (tick_interval <= 0.0f) return true; // non-ticking effect — affect() never fires

		tick_timer += dt;
		if (tick_timer >= tick_interval) {
			tick_timer -= tick_interval;
			return false; // tick fires — caller calls affect()
		}
		return true; // between ticks — skip affect()
	}

}
