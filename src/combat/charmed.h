#pragma once
#define CHARMED_H

#include "status-effect.h"

namespace engine::combat
{
	class charmed : public status_effect {
	public:
		charmed(math::fxpt const d, unsigned int intensity);

		void affect(util::indexed_storage< std::shared_ptr<entity> >::id_t const actor) override;
	};
}

