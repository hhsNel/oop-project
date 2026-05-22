#include "slowed.h"
#include "engine/actor.h"

namespace combat
{
	// intensity = procent spowolnienia


	void slowed::on_apply(engine::actor& target)
	{
		float slow_factor = 1.0f - (intensity / 100.0f);
		slow_factor = std::max(0.1f, slow_factor); // Spowolnienie może spowolnić prędkośc minimalnie do 10% orginalnej wartości
		target.movement_speed *= slow_factor;
	}


	void slowed::on_expire(engine::actor& target)
	{
		float slow_factor = 1.0f - (intensity / 100.0f);
		slow_factor = std::max(0.1f, slow_factor);
		target.movement_speed /= slow_factor;
	}
}
