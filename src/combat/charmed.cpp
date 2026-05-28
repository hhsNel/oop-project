#include "charmed.h"
#include "engine/actor.h"

namespace combat
{

	void charmed::on_apply(engine::actor& target)
	{
		target.team = engine::faction::player;
	}

	void charmed::on_expire(engine::actor& target)
	{
		target.team = engine::faction::enemy;
	}
}
