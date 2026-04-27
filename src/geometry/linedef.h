#pragma once
#define LINEDEF_H

#include "math/vec2.h"
#include "util/resource.h"
#include "util/indexed-storage.h"
#include <vector>

namespace geometry {
    class sidedef;

    class linedef {
    public:
        math::vec2 v1;
        math::vec2 v2;
        
        util::indexed_storage<sidedef>::id_t front;
        util::indexed_storage<sidedef>::id_t back;

        static std::vector<linedef> load_from_bin(util::resource const& res);
    };
}
