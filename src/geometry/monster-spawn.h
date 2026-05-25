#pragma once

#include <vector>
#include <cstdint>
#include "math/vec2.h"
#include "util/resource.h"

namespace geometry {
	struct monster_spawn {
		std::uint32_t type;
		math::vec2 pos;
		float z;

		monster_spawn(std::uint32_t t, math::vec2 p, float height)
			: type(t), pos(p), z(height) {}

		static std::vector<monster_spawn> load_from_bin(util::resource const& res);
	};
}
