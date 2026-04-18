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
		texture(std::vector<std::uint32_t> const &t, std::uint32_t const w, std::uint32_t const h);

		std::vector<std::uint32_t> pixels;
		std::uint32_t width, height;

		static texture const load_from_bin(util::resource const res);

		unsigned int get_offset(math::fxpt const stretched_len, math::fxpt const stretched_distance) const;
	};
}

