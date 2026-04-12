#pragma once
#define PROJECTILE_H

#include <memory>

#include "renderable-entity.h"
#include "math/fxpt.h"
#include "math/vec2.h"

namespace engine {
	class actor : virtual public renderable_entity {
		math::vec2 velocity;
	public:
		projectile(math::vec2 const v);

		virtual void update(math::fxpt const dt);

		virtual void on_traverse(math::fxpt const dt) = 0;
		virtual void on_hit(math::fxpt const dt, std::shared_ptr<rendering::renderable> const tgt, math::vec2 const collision_pt) = 0;
	};
}

