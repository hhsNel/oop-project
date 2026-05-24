#pragma once

#include <memory>

#include "math/vec2.h"

namespace engine {class actor;}

namespace combat
{
	class status_effect {
	protected:
		float duration;
		unsigned int intensity;
		float tick_interval;
		float tick_timer;
	public:
		status_effect(float const dur,float tick_inter, unsigned int intens)
			: duration(dur),intensity(intens), tick_interval(tick_inter), tick_timer(0.0f){}

		virtual void on_apply(engine::actor&) {}
		virtual void on_expire(engine::actor&) {}
		virtual void affect(engine::actor&) {}
		virtual ~status_effect() = default;

		bool tick(float dt);
		bool is_expired() const {return duration <= 0.0f;}
	};
}

