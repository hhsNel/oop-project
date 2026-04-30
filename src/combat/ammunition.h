
#pragma once
#define AMMUNITION_H

#include <memory>

#include "math/vec2.h"

namespace engine::combat
{
	class ammunition {
	public:
		virtual void spawn_bullet(math::vec2 const pos, float const angle) = 0;
		virtual ~ammunition() = default;
	};
}

