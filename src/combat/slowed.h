#pragma once
#define SLOWED_H

#include "status-effect.h"

namespace engine::combat
{
	class slowed : public status_effect {
	public:
		slowed(float const dur, unsigned int intens)
			: status_effect(dur, 0.0f, intens){}

		void on_apply(engine::actor& target) override;
		void on_expire(engine::actor& target) override;
	};
}

