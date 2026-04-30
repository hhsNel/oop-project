#include "geometry/subsector.h"

#include <utility>

namespace geometry {
    struct bin_subsector {
        std::uint32_t line_count;
        std::uint32_t first_line_id;
    } __attribute__((packed));

    std::vector<subsector> subsector::load_from_bin(util::resource const& res) {
        std::vector<subsector> result;
        size_t count = res.size / sizeof(bin_subsector);
        auto const* data = reinterpret_cast<bin_subsector const*>(res.begin);

        for (size_t i = 0; i < count; ++i) {
            subsector sub;
            for (std::uint32_t j = 0; j < data[i].line_count; ++j) {
                sub.lines.push_back(data[i].first_line_id + j); 
            }
            result.push_back(std::move(sub));
        }
        return result;
    }
}
