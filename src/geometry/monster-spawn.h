#pragma once

#include <vector>
#include <cstdint>
#include "math/vec2.h"
#include "util/resource.h"
#include "util/componentized.h"
#include "entities/monster-factory.h"

namespace geometry {
	class monster_spawn : public util::componentized<monster_spawn> {
		/* monster type id */
		std::uint32_t type;
		/* world position to spawn at */
		[[=util::component_field{}]] math::vec2 pos;
		/* spawn height */
		float z;

		friend class util::componentized<monster_spawn>;

	public:
		/* constructor */
		monster_spawn(std::uint32_t t, math::vec2 p, float height);

		/* parse a list of monster spawns from a binary resource */
		static std::vector<monster_spawn> load_from_bin(util::resource const& res);

		/* lets the factory read this spawn's private fields */
		friend std::unique_ptr<entities::monster> entities::make_monster(
			geometry::monster_spawn const& ms, engine::actor& target,
			geometry::map_data& map, engine::world& world);
	};
}

