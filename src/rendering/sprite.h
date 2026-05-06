#pragma once
#define SPRITE_H

#include "math/vec2.h"
#include "graphics/texture-manager.h"

namespace rendering {
	class sprite {
	public:
		math::vec2 pos;
		float z_pos;
		graphics::texture_set::texture_id tex_id;
		float inherent_scale;

		sprite(math::vec2 const p, float const z, graphics::texture_set::texture_id const tex, float const is);
		sprite();
		virtual ~sprite() = default;
	};
}

