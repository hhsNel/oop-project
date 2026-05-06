#include "texture-manager.h"
#include <sstream>

namespace graphics {
    texture_manager::texture_manager(util::resource_loader &resld, std::vector<texture_set> packs)
        : rl(resld), texture_sets(std::move(packs)), cur_set(0) {}

    texture_manager texture_manager::load(util::resource_loader &resld) {
        util::resource meta_res = *resld.lookup_resource("meta-texture-sets");
        
        std::vector<texture_set> packs;

        if (!meta_res.begin || meta_res.size == 0) {
            return texture_manager(resld, packs);
        }

        std::string content(reinterpret_cast<const char*>(meta_res.begin), meta_res.size);
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) {
                continue;
            }
                
			packs.push_back(texture_set::load(resld, line));
        }

        return texture_manager(resld, std::move(packs));
    }

	void texture_manager::cycle_set() {
		++cur_set;
		cur_set %= texture_sets.size();
	}

	texture const& texture_manager::wall_tx_by_id(texture_set::texture_id const id) const {
		return texture_sets[cur_set].wall_tx_by_id(id);
	}

	texture const& texture_manager::sprite_tx_by_id(texture_set::texture_id const id) const {
		return texture_sets[cur_set].sprite_tx_by_id(id);
	}

	texture const& texture_manager::flat_tx_by_id(texture_set::texture_id const id) const {
		return texture_sets[cur_set].flat_tx_by_id(id);
	}
}

