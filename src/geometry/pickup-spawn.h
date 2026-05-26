#pragma once

#include <vector>
#include <cstdint>
#include "math/vec2.h"
#include "util/resource.h"
#include "util/componentized.h"

namespace geometry {
	class pickup_spawn : public util::componentized<pickup_spawn> {
		[[=util::component_field{}]] std::uint32_t type;
		[[=util::component_field{}]] std::uint32_t subtype;
		[[=util::component_field{}]] math::vec2 pos;
		[[=util::component_field{}]] float z;

		friend class util::componentized<pickup_spawn>;

	public:
		pickup_spawn(std::uint32_t t, std::uint32_t st, math::vec2 p, float height)
			: type(t), subtype(st), pos(p), z(height) {}

		static std::vector<pickup_spawn> load_from_bin(util::resource const& res);
	};
}
