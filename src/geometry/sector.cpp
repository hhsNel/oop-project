#include "geometry/sector.h"
#include <cstring>

namespace geometry {
    struct bin_sector {
        float floor_height, ceiling_height;
        std::uint32_t floor_tex, ceiling_tex;
		std::uint8_t light_level;
    } __attribute__((packed));

    std::vector<sector> sector::load_from_bin(util::resource const& res) {
        std::vector<sector> result;
        if (!res.begin || res.size == 0) return result;

        size_t count = res.size / sizeof(bin_sector);
        auto const* data = reinterpret_cast<bin_sector const*>(res.begin);

        for (size_t i = 0; i < count; ++i) {
            sector s;
            s.floor_height = float(data[i].floor_height);
            s.ceiling_height = float(data[i].ceiling_height);
            s.floor_tex = data[i].floor_tex;
            s.ceiling_tex = data[i].ceiling_tex;
            s.light_level = data[i].light_level;
            result.push_back(s);
        }
        return result;
    }
}
