#include "map-data.h"
#include <algorithm>

namespace geometry {

    util::indexed_storage<subsector>::id_t map_data::get_subsector_id(math::vec2 const& pt) const {
        auto curr_id = root_node_id;
        
        while (true) {
            auto const& node = nodes[curr_id];
            
            if (node.is_pt_front_side(pt)) {
                if (node.is_front_leaf()) {
                    return node.get_front_index();
                }
                curr_id = node.get_front_index();
            } else {
                if (node.is_back_leaf()) {
                    return node.get_back_index();
                }
                curr_id = node.get_back_index();
            }
        }
    }

    void map_data::move_to(rendering::sprite* spr, math::vec2 const& new_pos) {
        if (!spr) return;

        auto old_sub_id = get_subsector_id(spr->pos);
        auto new_sub_id = get_subsector_id(new_pos);

        if (old_sub_id == new_sub_id) {
            spr->pos = new_pos;
            return;
        }

        auto& old_sub = subsectors[old_sub_id];
        auto& new_sub = subsectors[new_sub_id];

        auto it = std::find_if(old_sub.sprites.begin(), old_sub.sprites.end(),
            [spr](std::unique_ptr<rendering::sprite> const& p) {
                return p.get() == spr;
            });

        if (it != old_sub.sprites.end()) {
            std::unique_ptr<rendering::sprite> moved_spr = std::move(*it);
            old_sub.sprites.erase(it);
            
            moved_spr->pos = new_pos;
            
            new_sub.sprites.push_back(std::move(moved_spr));
        } else {
            spr->pos = new_pos;
        }
    }

	map_data map_data::load_from_bin(util::resource const& sectors_res, util::resource const& sidedefs_res, util::resource const& linedefs_res, util::resource const& subsectors_res, util::resource const& nodes_res) {
        map_data map;

        map.sectors = util::indexed_storage<sector>(sector::load_from_bin(sectors_res));
        map.sidedefs = util::indexed_storage<sidedef>(sidedef::load_from_bin(sidedefs_res));
        map.linedefs = util::indexed_storage<linedef>(linedef::load_from_bin(linedefs_res));
        map.subsectors = util::indexed_storage<subsector>(subsector::load_from_bin(subsectors_res));

        auto loaded_nodes = util::indexed_storage<bsp_node>(bsp_node::load_from_bin(nodes_res));

        if (!loaded_nodes.size() == 0) {
            map.root_node_id = loaded_nodes.size();
        } else {
            map.root_node_id = 0;
        }

        map.nodes = std::move(loaded_nodes);

        return map;
    }
}
