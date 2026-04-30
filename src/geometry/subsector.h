#pragma once
#define SUBSECTOR_H

#include "util/resource.h"
#include "util/indexed-storage.h"
#include "rendering/sprite.h"
#include <vector>
#include <memory>

namespace geometry {
    class linedef;

    class subsector {
    public:
        std::vector<util::indexed_storage<linedef>::id_t> lines;
		/* TODO: what owns the sprites? */
		std::vector<std::unique_ptr<rendering::sprite> > sprites;

        static std::vector<subsector> load_from_bin(util::resource const& res);
    };
}
