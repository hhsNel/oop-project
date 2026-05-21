#include "asset-pack.h"
#include <sstream>
#include <string>
#include <unordered_map>

namespace assets {

    asset_pack::asset_pack(std::vector<texture> walls, std::vector<texture> sprites, std::vector<texture> flats, std::vector<texture> ui, std::vector<menu_element> menu_elems, std::vector<menu> ms) :
		wall_textures(std::move(walls)),
        sprite_textures(std::move(sprites)),
        flat_textures(std::move(flats)),
        ui_textures(std::move(ui)),
        menu_elements(std::move(menu_elems)),
        menus(std::move(ms)) {}

    texture const& asset_pack::wall_tx_by_id(texture_id const id) const {
        return wall_textures.at(id);
    }

    texture const& asset_pack::sprite_tx_by_id(texture_id const id) const {
        return sprite_textures.at(id);
    }

    texture const& asset_pack::flat_tx_by_id(texture_id const id) const {
        return flat_textures.at(id);
    }

    texture const& asset_pack::ui_tx_by_id(texture_id const id) const {
        return ui_textures.at(id);
    }

    menu &asset_pack::menu_by_id(menu_id const id) {
        return menus.at(id);
    }

    std::vector<texture> asset_pack::tx_from_meta(util::resource_loader &resld, std::string_view meta_path) {
        auto r = resld.lookup_resource(meta_path);
        if (!r) {
            return {};
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return {};
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        std::istringstream stream(content);
        std::string line;
        std::vector<texture> textures;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            auto tr = resld.lookup_resource(line);
            if (tr) {
                textures.push_back(texture::load_from_bin(*tr));
            }
        }

        return textures;
    }

    std::vector<menu_element> asset_pack::menu_elems_from_meta(util::resource_loader &resld, std::string_view meta_path, std::vector<texture> const& ui_tx) {
        auto r = resld.lookup_resource(meta_path);
        if (!r) {
            return {};
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return {};
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        std::istringstream stream(content);
        std::string line;
        std::vector<menu_element> elements;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            auto er = resld.lookup_resource(line);
            if (er) {
                elements.push_back(menu_element::load_from_bin(*er, ui_tx));
            }
        }

        return elements;
    }

    std::vector<menu> asset_pack::menus_from_meta(util::resource_loader &resld, std::string_view meta_path, std::vector<texture> const& ui_tx) {
        auto r = resld.lookup_resource(meta_path);
        if (!r) {
            return {};
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return {};
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        std::istringstream stream(content);
        std::string line;
        std::vector<menu> result;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            auto mr = resld.lookup_resource(line);
            if (mr) {
                result.push_back(menu::load_from_bin(resld, *mr, ui_tx));
            }
        }

        return result;
    }

    std::unordered_map<std::string, std::string> asset_pack::parse_meta_keys(std::string const& content) {
        std::unordered_map<std::string, std::string> keys;
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            auto colon = line.find(':');
            if (colon == std::string::npos) {
                continue;
            }

            std::string key   = line.substr(0, colon);
            std::string value = line.substr(colon + 1);

            auto trim = [](std::string &s) {
                std::size_t start = s.find_first_not_of(" \t");
                std::size_t end   = s.find_last_not_of(" \t");
                s = (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
            };
            trim(key);
            trim(value);

            if (!key.empty()) {
                keys[key] = value;
            }
        }

        return keys;
    }

    asset_pack asset_pack::load(util::resource_loader &resld, std::string_view pack_meta_path) {
        auto r = resld.lookup_resource(pack_meta_path);
        if (!r) {
            return asset_pack({}, {}, {}, {}, {}, {});
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return asset_pack({}, {}, {}, {}, {}, {});
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        auto keys = parse_meta_keys(content);

        std::vector<texture>      walls    = keys.count("wall")          ? tx_from_meta(resld, keys.at("wall"))                           : std::vector<texture>{};
        std::vector<texture>      sprites  = keys.count("sprite")        ? tx_from_meta(resld, keys.at("sprite"))                         : std::vector<texture>{};
        std::vector<texture>      flats    = keys.count("flat")          ? tx_from_meta(resld, keys.at("flat"))                           : std::vector<texture>{};
        std::vector<texture>      ui       = keys.count("ui")            ? tx_from_meta(resld, keys.at("ui"))                             : std::vector<texture>{};
        std::vector<menu_element> menu_els = keys.count("menu-elements") ? menu_elems_from_meta(resld, keys.at("menu-elements"), ui)      : std::vector<menu_element>{};
        std::vector<menu>         menus    = keys.count("menus")         ? menus_from_meta(resld, keys.at("menus"), ui)                   : std::vector<menu>{};

        return asset_pack(
            std::move(walls), std::move(sprites), std::move(flats),
            std::move(ui), std::move(menu_els), std::move(menus)
        );
    }
}
