#pragma once
#define TEXTURE_H

#include <vector>
#include <cstddef>
#include <cstdint>

#include "util/resource.h"
#include "util/componentized.h"

namespace assets {
	class texture : public util::componentized<texture> {
		texture(std::vector<std::uint32_t> const &t, std::uint32_t const w, std::uint32_t const h, bool const transparent = false);

		[[=util::component_field{}]] std::vector<std::uint32_t> pixels;
		[[=util::component_field{}]] std::uint32_t width, height;

		[[=util::component_field{}]] bool has_transparency; 

	public:
		static texture const load_from_bin(util::resource const &res);

		friend util::componentized<texture>;
	};
}
