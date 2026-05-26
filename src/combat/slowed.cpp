#include "slowed.h"
#include "engine/actor.h"
#include <algorithm>

namespace combat
{
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
