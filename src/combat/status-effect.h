#pragma once
#define STATUS_EFFECT_H

#include <memory>

#include "math/fxpt.h"
#include "engine/actor.h"
#include "util/indexed-storage.h"

namespace engine::combat
{
	class status_effect {
		math::fxpt duration;
		unsigned int intensity;
		math::fxpt tick_timer;
	public:
		status_effect(math::fxpt const d, unsigned int intensity);

		virtual void affect(util::indexed_storage< std::shared_ptr<entity> >::id_t const actor) = 0;
	};
}

