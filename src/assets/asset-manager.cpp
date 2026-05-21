#include "asset-manager.h"
#include "rendering/renderer-2d.h"
#include "rendering/rendering-backend.h"

#include <sstream>

namespace assets {

    asset_manager::asset_manager(util::resource_loader &resld, std::vector<asset_pack> packs)
        : rl(resld), asset_packs(std::move(packs)), cur_set(0) {}

    asset_manager asset_manager::load(util::resource_loader &resld) {
        std::vector<asset_pack> packs;

        auto r = resld.lookup_resource("meta-texture-sets");
        if (!r) {
            return asset_manager(resld, packs);
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return asset_manager(resld, packs);
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            packs.push_back(asset_pack::load(resld, line));
        }

        return asset_manager(resld, std::move(packs));
    }

    void asset_manager::cycle_set() {
        if (asset_packs.empty()) {
            return;
        }
        cur_set = (cur_set + 1) % static_cast<unsigned int>(asset_packs.size());
    }

    texture const& asset_manager::wall_tx_by_id(texture_id const id) const {
        return asset_packs[cur_set].wall_tx_by_id(id);
    }

    texture const& asset_manager::sprite_tx_by_id(texture_id const id) const {
        return asset_packs[cur_set].sprite_tx_by_id(id);
    }

    texture const& asset_manager::flat_tx_by_id(texture_id const id) const {
        return asset_packs[cur_set].flat_tx_by_id(id);
    }

    texture const& asset_manager::ui_tx_by_id(texture_id const id) const {
        return asset_packs[cur_set].ui_tx_by_id(id);
    }

    menu &asset_manager::menu_by_id(menu_id const id) {
        return asset_packs[cur_set].menu_by_id(id);
    }

    int asset_manager::display_menu(menu_id const id, rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend &in) {
        return asset_packs[cur_set].display_menu(id, r2d, rb, in);
    }
}
