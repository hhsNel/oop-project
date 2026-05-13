#include "texture-set.h"
#include <sstream>
#include <string>

namespace graphics {
    texture_set::texture_set(std::vector<texture> const& walls, std::vector<texture> const& sprites, std::vector<texture> const& flat)
        : wall_textures(walls), sprite_textures(sprites), flat_textures(flat) {}

    texture const& texture_set::wall_tx_by_id(texture_id const id) const {
        return wall_textures.at(id);
    }

    texture const& texture_set::sprite_tx_by_id(texture_id const id) const {
        return sprite_textures.at(id);
    }

    texture const& texture_set::flat_tx_by_id(texture_id const id) const {
        return flat_textures.at(id);
    }

    std::vector<texture> texture_set::tx_from_meta(util::resource_loader &resld, std::string_view meta_path) {
        auto r = resld.lookup_resource(meta_path);
		if(!r) {
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
			if(tr) {
				util::resource &tex_res = *tr;
				textures.push_back(texture::load_from_bin(tex_res));
			}
        }

        return textures;
    }

    texture_set texture_set::load(util::resource_loader &resld, std::string_view pack_meta_path) {
        auto r = resld.lookup_resource(pack_meta_path);
        if (!r) {
            return texture_set({}, {}, {});
        }

        util::resource &meta_res = *r;

        if (!meta_res("beginning"_f) || meta_res("size"_f) == 0) {
            return texture_set({}, {}, {});
        }

        std::string content(reinterpret_cast<const char*>(meta_res("beginning"_f)), meta_res("size"_f));
        std::istringstream stream(content);
        std::string line;
        std::vector<std::string> meta_paths;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (!line.empty()) {
                meta_paths.push_back(line);
            }
        }

        std::vector<texture> walls   = meta_paths.size() > 1 ? tx_from_meta(resld, meta_paths[0]) : std::vector<texture>{};
        std::vector<texture> sprites = meta_paths.size() > 1 ? tx_from_meta(resld, meta_paths[1]) : std::vector<texture>{};
        std::vector<texture> flats   = meta_paths.size() > 2 ? tx_from_meta(resld, meta_paths[2]) : std::vector<texture>{};

        return texture_set(walls, sprites, flats);
    }
}
