#include "sprite.h"

rendering::sprite::sprite(math::vec2 const p, float const z, graphics::texture_set::texture_id const tex, float const is) :
	pos(p),
	z_pos(z),
	tex_id(tex),
	inherent_scale(is) { }

rendering::sprite::sprite() :
	pos(math::vec2(0.0f, 0.0f)),
	z_pos(0.0f),
	tex_id(-1),
	inherent_scale(0.0f) { }

