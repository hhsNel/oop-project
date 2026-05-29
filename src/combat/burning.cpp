#include "burning.h"
#include "engine/actor.h"

namespace combat
{
	burning::burning(float const dur, unsigned int intens)
		: status_effect(dur, 0.5f, intens) {}

	void burning::affect(engine::actor& target)
	{
		float damage = static_cast<float>(intensity);

		target.take_true_damage(damage);
	}
}
