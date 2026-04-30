#pragma once
#define CHARMED_H

#include "status-effect.h"

namespace engine::combat
{
	class charmed : public status_effect {
	public:
		charmed(float const dur, unsigned int intens)
			: status_effect(dur, 0.0f, intens){}

		void on_apply(engine::actor& target) override;
		void on_expire(engine::actor& target) override;
	};
}

