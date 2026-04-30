#pragma once
#define PROJECTILE_H

#include <memory>

#include "renderable-entity.h"
#include "rendering/renderable.h"
#include "math/vec2.h"

namespace engine {
	class projectile : virtual public renderable_entity {
		math::vec2 velocity;
	public:
		projectile(math::vec2 const v);

		virtual void update(float const dt);

		virtual void on_traverse(float const dt) = 0;
		virtual void on_hit(float const dt, std::shared_ptr<rendering::renderable> const tgt, math::vec2 const collision_pt) = 0;
	};
}

