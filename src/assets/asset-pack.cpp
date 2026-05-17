#include "asset-pack.h"
#include <sstream>
#include <string>
#include <unordered_map>

namespace assets {

    asset_pack::asset_pack(
        std::vector<texture> const& walls,
        std::vector<texture> const& sprites,
        std::vector<texture> const& flats,
        std::vector<texture> const& ui,
        std::vector<menu_element> const& menu_elems
    ) : wall_textures(walls),
        sprite_textures(sprites),
        flat_textures(flats),
        ui_backgrounds(ui),
        menu_elements(menu_elems) {}

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
        return ui_backgrounds.at(id);
    }

    menu_element const& asset_pack::menu_elem_by_id(element_id const id) const {
        return menu_elements.at(id);
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
                util::resource &tex_res = *tr;
                textures.push_back(texture::load_from_bin(tex_res));
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
                util::resource &elem_res = *er;
                elements.push_back(menu_element::load_from_bin(elem_res, ui_tx));
            }
        }

        return elements;
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

            std::string key = line.substr(0, colon);
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
            return asset_pack({}, {}, {}, {}, {});
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return asset_pack({}, {}, {}, {}, {});
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        auto keys = parse_meta_keys(content);

        std::vector<texture>      walls    = keys.count("wall")           ? tx_from_meta(resld, keys.at("wall"))                      : std::vector<texture>{};
        std::vector<texture>      sprites  = keys.count("sprite")         ? tx_from_meta(resld, keys.at("sprite"))                    : std::vector<texture>{};
        std::vector<texture>      flats    = keys.count("flat")           ? tx_from_meta(resld, keys.at("flat"))                      : std::vector<texture>{};
        std::vector<texture>      ui       = keys.count("ui")             ? tx_from_meta(resld, keys.at("ui"))                        : std::vector<texture>{};
        std::vector<menu_element> menu_els = keys.count("menu-elements")  ? menu_elems_from_meta(resld, keys.at("menu-elements"), ui) : std::vector<menu_element>{};

        return asset_pack(walls, sprites, flats, ui, menu_els);
    }
}
