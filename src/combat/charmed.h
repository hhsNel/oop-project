#pragma once

#include "status-effect.h"

namespace combat
{
	class charmed : public status_effect {
	public:
		/* constructor */
		charmed(float const dur, unsigned int intens);

		/* turn the target to the player's side when applied */
		void on_apply(engine::actor& target) override;
		/* return the target to the enemy faction when expired */
		void on_expire(engine::actor& target) override;
	};
}

