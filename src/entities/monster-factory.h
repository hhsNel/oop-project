#pragma once

#include <memory>
#include <cstdint>
#include "math/vec2.h"
#include "entities/monster.h"

namespace engine { class world; }

namespace geometry {
    class monster_spawn;
    class map_data;
}

namespace entities {
	/* build a monster instance from a spawn descriptor */
	std::unique_ptr<monster> make_monster(
		geometry::monster_spawn const& ms, engine::actor& target,
		geometry::map_data& map, engine::world& world);
}

