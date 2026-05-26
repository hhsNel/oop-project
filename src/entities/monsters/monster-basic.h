#pragma once

#include "entities/monster.h"

namespace entities {

	class monster_basic : public monster {
	public:
		monster_basic(math::vec2 const p, float const z, engine::actor* target = nullptr);
		void update(float dt) override;
	};

}
