#include "charmed.h"
#include "engine/actor.h"

namespace engine::combat
{
	//charmed zmienia frakcję celu na gracza (player) przy nałożeniu,
	//przywraca orginalną frakcję po wygaśnięciu
	//tick_interval = 0.0f - efekt nie tickuje, działa wyłącznie przez on_apply/on_expire

	void charmed::on_apply(engine::actor& target)
	{
		target.team = faction::player;
	}

	void charmed::on_expire(engine::actor& target)
	{
		target.team = faction::enemy;
	}
}
