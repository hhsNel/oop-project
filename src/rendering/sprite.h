#pragma once
#define SPRITE_H

#include "math/vec2.h"
#include "assets/ids.h"

namespace rendering {
	class sprite {
		/* TODO */
	public:
		math::vec2 pos;
		float z_pos;
		assets::texture_id tex_id;
		float inherent_scale;

		sprite(math::vec2 const p, float const z, assets::texture_id const tex, float const is);
		sprite();
		virtual ~sprite() = default;
	};
}

