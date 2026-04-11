#pragma once
#define BURNING_H

#include "status-effect.h"

namespace engine::combat
{
	class burning : public status_effect {
	public:
		burning(math::fxpt const d, unsigned int intensity);

		void affect(util::indexed_storage< std::shared_ptr<entity> >::id_t const actor) override;
	};
}

