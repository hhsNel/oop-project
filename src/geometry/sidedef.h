#pragma once
#define SIDEDEF_H

#include "assets/asset-manager.h"
#include "util/resource.h"
#include "util/indexed-storage.h"
#include <vector>

namespace geometry {
    class sector; 

    class sidedef {
    public:
        util::indexed_storage<sector>::id_t facing_sector; 
        
        assets::asset_pack::texture_id upper_tex;
        assets::asset_pack::texture_id middle_tex;
        assets::asset_pack::texture_id lower_tex;

        static std::vector<sidedef> load_from_bin(util::resource const& res);
    };
}
