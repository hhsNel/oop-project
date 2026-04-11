#pragma once
#define TEXTURE_H

#include <vector>
#include <stddef>

#include "util/resource.h"
#include "math/fxpt.h"

namespace graphics {
	class texture {
		std::vector<uint32_t> texture;
		unsigned int width, height;

		texture(std::vector<uint32_t> const &t, unsigned int const w, unsigned int const h);
	public:
		static texture const load_from_bin(util::resource const res);

		unsigned int const get_width() const;
		unsigned int const get_height() const;
		std::vector<uint32_t> const &get_texture() const;
		unsigned int const get_offset(math::fxpt const stretched_len, math::fxpt const stretched_distance) const;
	};
}

