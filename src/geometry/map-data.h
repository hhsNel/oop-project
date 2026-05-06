#pragma once
#define MAP_DATA_H

#include "sector.h"
#include "sidedef.h"
#include "linedef.h"
#include "subsector.h"
#include "bsp-node.h"
#include "rendering/sprite.h"
#include "util/indexed-storage.h"

namespace geometry {
    class map_data {
    public:
        util::indexed_storage<sector> sectors;
        util::indexed_storage<sidedef> sidedefs;
        util::indexed_storage<linedef> linedefs;
        util::indexed_storage<subsector> subsectors;
        util::indexed_storage<bsp_node> nodes;

        util::indexed_storage<bsp_node>::id_t root_node_id;

        util::indexed_storage<subsector>::id_t get_subsector_id(math::vec2 const& pt) const;

        void move_to(rendering::sprite* spr, math::vec2 const& new_pos);

        static map_data load_from_bin(util::resource const& sectors_res, util::resource const& sidedefs_res, util::resource const& linedefs_res, util::resource const& subsectors_res, util::resource const& nodes_res);
    };
}

