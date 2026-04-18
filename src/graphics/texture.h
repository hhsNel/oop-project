#pragma once
#define TEXTURE_H

#include <vector>
#include <cstddef>
#include <cstdint>

#include "util/resource.h"
#include "math/fxpt.h"

namespace graphics {
	class texture {
	public:
		texture(std::vector<std::uint32_t> const &t, unsigned int const w, unsigned int const h);

		std::vector<std::uint32_t> pixels;
		unsigned int width, height;

		static texture const load_from_bin(util::resource const res);

		unsigned int get_offset(math::fxpt const stretched_len, math::fxpt const stretched_distance) const;
	};
}

