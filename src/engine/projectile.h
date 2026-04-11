#pragma once
#define PROJECTILE_H

#include "renderable-entity.h"
#include "rendering/renderable.h"
#include "math/fxpt.h"
#include "math/vec2.h"

namespace engine {
	class actor : virtual public renderable_entity {
		math::vec2 velocity;
	public:
		projectile(math::vec v);

		virtual void update(math::fxpt dt);

		virtual void on_traverse(math::fxpt dt) = 0;
		virtual void on_hit(math::fxpt dt, rendering::renderable *tgt, math::vec2 collision_pt) = 0;
	};
}

