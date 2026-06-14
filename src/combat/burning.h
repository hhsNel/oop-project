#pragma once

#include "status-effect.h"

namespace combat
{
	class burning : public status_effect {
	public:
		/* constructor */
		burning(float const dur, unsigned int intens);

		/* deal burn damage to the target on each tick */
		void affect(engine::actor& target) override;
	};
}

