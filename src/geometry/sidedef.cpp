#include "geometry/sidedef.h"

namespace geometry {
    struct bin_sidedef {
        std::uint32_t facing_sector;
        std::uint32_t upper_tex, middle_tex, lower_tex;
    } __attribute__((packed));

    std::vector<sidedef> sidedef::load_from_bin(util::resource const& res) {
        std::vector<sidedef> result;
        size_t count = res.size / sizeof(bin_sidedef);
        auto const* data = reinterpret_cast<bin_sidedef const*>(res.begin);

        for (size_t i = 0; i < count; ++i) {
            sidedef sd;
            sd.facing_sector = data[i].facing_sector;
            sd.upper_tex = data[i].upper_tex;
            sd.middle_tex = data[i].middle_tex;
            sd.lower_tex = data[i].lower_tex;
            result.push_back(sd);
        }
        return result;
    }
}