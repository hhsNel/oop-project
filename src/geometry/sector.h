#pragma once
#define SECTOR_H

#include "graphics/texture-manager.h"
#include "util/resource.h"
#include <vector>
#include <cstdint>

namespace geometry {
    class sector {
    public:
        float floor_height;
        float ceiling_height;
        graphics::texture_manager::texture_id floor_tex;
        graphics::texture_manager::texture_id ceiling_tex;
		std::uint8_t light_level;

        static std::vector<sector> load_from_bin(util::resource const& res);
    };
}
