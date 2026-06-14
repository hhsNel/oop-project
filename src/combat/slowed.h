#pragma once

#include "status-effect.h"

namespace combat
{
	class slowed : public status_effect {
	public:
		/* constructor */
		slowed(float const dur, unsigned int intens);

		/* reduce the target's movement speed when applied */
		void on_apply(engine::actor& target) override;
		/* restore the target's movement speed when expired */
		void on_expire(engine::actor& target) override;
	};
}

