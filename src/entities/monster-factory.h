#pragma once

#include <memory>
#include <cstdint>
#include "math/vec2.h"
#include "entities/monster.h"

namespace entities {
	std::unique_ptr<monster> make_monster(std::uint32_t type, math::vec2 pos, float z);
}
