#include "charmed.h"
#include "engine/actor.h"

namespace combat
{
	charmed::charmed(float const dur, unsigned int intens)
		: status_effect(dur, 0.0f, intens) {}

	void charmed::on_apply(engine::actor& target)
	{
		target.team = engine::faction::player;
	}

	void charmed::on_expire(engine::actor& target)
	{
		target.team = engine::faction::enemy;
	}
}
