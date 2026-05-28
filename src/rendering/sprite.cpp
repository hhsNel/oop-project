#include "sprite.h"

rendering::sprite::sprite(math::vec2 const p, float const z, assets::texture_id const tex, float const is) :
	pos(p),
	z_pos(z),
	angle(0.0f),
	tex_id(tex),
	inherent_scale(is) { }

