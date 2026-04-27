#include "visplane.h"

rendering::visplane::visplane(int const x, unsigned int const sw, float const h, graphics::texture_manager::texture_id const tid, std::uint8_t const light) :
	height(h),
	tex_id(tid),
	light_level(light),
	min_x(x),
	max_x(x) {
	top.assign(sw, -1);
	bottom.assign(sw, -1);
}

