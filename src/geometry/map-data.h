#pragma once
#define MAP_DATA_H

#include "sector.h"
#include "sidedef.h"
#include "linedef.h"
#include "subsector.h"
#include "bsp-node.h"
#include "util/indexed-storage.h"

namespace geometry {
    struct map_data {
        util::indexed_storage<sector> sectors;
        util::indexed_storage<sidedef> sidedefs;
        util::indexed_storage<linedef> linedefs;
        util::indexed_storage<subsector> subsectors;
        util::indexed_storage<bsp_node> nodes;
        
        util::indexed_storage<bsp_node>::id_t root_node_id; 
    };
}
