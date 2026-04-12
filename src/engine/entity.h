#pragma once
#define ENTITY_H

#include "math/fxpt.h"

namespace engine {
	class entity {
	public:
		virtual void update(math::fxpt const dt) = 0;
		virtual ~entity = default;
	};
}

