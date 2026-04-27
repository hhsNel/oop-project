#pragma once
#define TEXTURE_H

#include <vector>
#include <cstddef>
#include <cstdint>

#include "util/resource.h"

namespace graphics {
	class texture {
		texture(std::vector<std::uint32_t> const &t, std::uint32_t const w, std::uint32_t const h, bool const transparent = false);

	public:
		std::vector<std::uint32_t> pixels;
		std::uint32_t width, height;
		
		bool has_transparency; 

		static texture const load_from_bin(util::resource const res);
	};
}
