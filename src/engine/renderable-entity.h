#pragma once
#include "entity.h"
#include "rendering/sprite.h"

namespace engine {
	class renderable_entity : public entity, public rendering::sprite {
	public:
		/* constructor */
		renderable_entity(math::vec2 const p, float const z, assets::texture_id const tex, float const is)
			: entity(), rendering::sprite(p, z, tex, is) {}

		//renderable_entity() : rendering::sprite() {}

		/* virtual destructor */
		virtual ~renderable_entity() = default;
	};
}

