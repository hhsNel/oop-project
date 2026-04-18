#include "texture-manager.h"
#include "util/resource-loader.h"

#include <sstream>
#include <string>

namespace graphics {
    texture_manager::texture_manager(std::vector<texture> const& sprites, std::vector<texture> const& flat)
        : sprite_textures(sprites), flat_textures(flat) {}

    texture const& texture_manager::sprite_tx_by_id(texture_id const id) const {
        return sprite_textures.at(id);
    }

    texture const& texture_manager::flat_tx_by_id(texture_id const id) const {
        return flat_textures.at(id);
    }

	std::vector<texture> texture_manager::tx_from_meta(std::string_view meta_path) {
        util::resource meta_res = util::resource_loader::lookup_resource(meta_path);

        if (!meta_res.begin || meta_res.size == 0) {
            return {};
        }

        std::string content(reinterpret_cast<const char*>(meta_res.begin), meta_res.size);
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

            util::resource tex_res = util::resource_loader::lookup_resource(line);

            textures.push_back(texture::load_from_bin(tex_res));
        }

        return textures;
    }

    texture_manager const texture_manager::load() {
        std::vector<texture> sprites = tx_from_meta("meta/sprite");
        std::vector<texture> flats   = tx_from_meta("meta/flat");

        return texture_manager(sprites, flats);
    }
}
