#include "geometry/linedef.h"

namespace geometry {
    struct bin_linedef {
        int v1_x, v1_y, v2_x, v2_y;
        std::uint32_t front, back;
    } __attribute__((packed));

    std::vector<linedef> linedef::load_from_bin(util::resource const& res) {
        std::vector<linedef> result;
        size_t count = res.size / sizeof(bin_linedef);
        auto const* data = reinterpret_cast<bin_linedef const*>(res.begin);

        for (size_t i = 0; i < count; ++i) {
            linedef ld;
            ld.v1 = math::vec2(float(data[i].v1_x), float(data[i].v1_y));
            ld.v2 = math::vec2(float(data[i].v2_x), float(data[i].v2_y));
            ld.front = data[i].front;
            ld.back = data[i].back;
            result.push_back(ld);
        }
        return result;
    }
}
