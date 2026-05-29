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
	std::unique_ptr<monster> make_monster(std::uint32_t type, math::vec2 pos, float z, engine::actor& target, geometry::map_data& map, engine::world& world);
}

