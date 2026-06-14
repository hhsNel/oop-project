#pragma once

#include "math/vec2.h"

namespace engine {class actor;}

namespace combat
{
	class status_effect {
	protected:
		/* remaining time before the effect expires */
		float duration;
		/* strength of the effect */
		unsigned int intensity;
		/* time between effect ticks */
		float tick_interval;
		/* time accumulated toward the next tick */
		float tick_timer;
	public:
		/* constructor */
		status_effect(float const dur, float tick_inter, unsigned int intens);

		/* called once when the effect is applied to an actor */
		virtual void on_apply(engine::actor&);
		/* called once when the effect expires */
		virtual void on_expire(engine::actor&);
		/* called whenever the effect ticks */
		virtual void affect(engine::actor&);
		/* virtual destructor */
		virtual ~status_effect() = default;

		/* advance timers, returns true while between ticks (skip affect) */
		bool tick(float dt);
		/* whether the effect has run its full duration */
		bool is_expired() const;
	};
}

