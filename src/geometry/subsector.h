#pragma once
#define SUBSECTOR_H

#include "util/resource.h"
#include "util/indexed-storage.h"
#include <vector>

namespace geometry {
    class linedef;

    class subsector {
    public:
        std::vector<util::indexed_storage<linedef>::id_t> lines;

        static std::vector<subsector> load_from_bin(util::resource const& res);
    };
}
