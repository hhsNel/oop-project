#include "status-effect.h"

namespace combat {

	status_effect::status_effect(float const dur, float tick_inter, unsigned int intens)
		: duration(dur), intensity(intens), tick_interval(tick_inter), tick_timer(0.0f) {}

	void status_effect::on_apply(engine::actor&) {}
	void status_effect::on_expire(engine::actor&) {}
	void status_effect::affect(engine::actor&) {}

	bool status_effect::is_expired() const { return duration <= 0.0f; }

	bool status_effect::tick(float dt) {
		duration -= dt;
		if (tick_interval <= 0.0f) return true; // non-ticking effect

		tick_timer += dt;
		if (tick_timer >= tick_interval) {
			tick_timer -= tick_interval;
			return false; // tick fires
		}
		return true; // between ticks — skip affect()
	}

}
