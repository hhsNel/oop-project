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
	std::unique_ptr<monster> make_monster(geometry::monster_spawn const& ms, engine::actor* target = nullptr, geometry::map_data* map = nullptr, engine::world* world = nullptr);
}

