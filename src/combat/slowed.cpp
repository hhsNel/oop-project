#include "slowed.h"
#include "engine/actor.h"
#include <algorithm>

namespace combat
{
	slowed::slowed(float const dur, unsigned int intens)
		: status_effect(dur, 0.0f, intens) {}

	void slowed::on_apply(engine::actor& target)
	{
		float slow_factor = std::clamp(1.0f - (intensity / 100.0f), 0.1f, 1.0f);
		target.movement_speed *= slow_factor;
	}


	void slowed::on_expire(engine::actor& target)
	{
		float slow_factor = std::clamp(1.0f - (intensity / 100.0f), 0.1f, 1.0f);
		target.movement_speed /= slow_factor;
	}
}
