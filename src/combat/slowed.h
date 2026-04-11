#pragma once
#define SLOWED_H

#include "status-effect.h"

namespace engine::combat
{
	class slowed : public status_effect {
	public:
		slowed(math::fxpt const d, unsigned int intensity);

		void affect(util::indexed_storage< std::shared_ptr<entity> >::id_t const actor) override;
	};
}

