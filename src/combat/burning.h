#pragma once
#define BURNING_H

#include "status-effect.h"

namespace engine::combat
{
	class burning : public status_effect {
	public:
		burning(float const dur, unsigned int intens)
			: status_effect(dur, 0.5f, intens){}

		void affect(engine::actor& target) override;
	};
}

