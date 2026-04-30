#include "burning.h"
#include "engine/actor.h"

namespace engine::combat
{
	void burning::affect(engine::actor& target)
	{
		float damage = static_cast<float>(intensity);

		target.take_true_damage(damage);
	}
}
