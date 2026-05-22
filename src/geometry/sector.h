#pragma once
#define SECTOR_H

#include "assets/asset-manager.h"
#include "util/resource.h"
#include <vector>
#include <cstdint>

namespace geometry {
    class sector {
    public:
        float floor_height;
        float ceiling_height;
        assets::texture_id floor_tex;
        assets::texture_id ceiling_tex;
		std::uint8_t light_level;

        static std::vector<sector> load_from_bin(util::resource const& res);
    };
}
