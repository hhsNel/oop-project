#include "vissprite.h"

rendering::vissprite::vissprite(sprite const& sprite, float const z, int const clamped_x1, int const clamped_x2, float const px, float const sc, std::uint8_t const light, std::vector<int> const& uc, std::vector<int> const& lc) :
	depth(z),
	cx1(clamped_x1),
	cx2(clamped_x2),
	proj_x(px),
	scale(sc),
	z_pos(sprite.z_pos),
	tex_id(sprite.tex_id),
	light_level(light),
	upper_clip(uc),
	lower_clip(lc) { }

