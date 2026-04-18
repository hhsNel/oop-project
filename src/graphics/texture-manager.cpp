#include "texture-manager.h"

namespace graphics {

    texture_manager::texture_manager(std::vector<texture> const& sprites, std::vector<texture> const& flat)
        : sprite_textures(sprites), flat_textures(flat) {}

    texture_manager const texture_manager::load_from_bin(util::resource const res) {
        const std::uint32_t* ptr = reinterpret_cast<const std::uint32_t*>(res.begin);
        const std::uint32_t* end = reinterpret_cast<const std::uint32_t*>(res.end);

        if (ptr >= end) return texture_manager({}, {});

        std::uint32_t num_sprites = *ptr++;
        std::uint32_t num_flats = *ptr++;

        std::vector<texture> sprites = read_textures(num_sprites, &ptr, end);
        std::vector<texture> flats = read_textures(num_flats, &ptr, end);

        return texture_manager(sprites, flats);
    }

    texture const& texture_manager::sprite_tx_by_id(texture_id const id) const {
        return sprite_textures[id];
    }

    texture const& texture_manager::flat_tx_by_id(texture_id const id) const {
        return flat_textures[id];
    }

	std::vector<texture> read_textures(std::uint32_t count, const std::uint32_t **ptr, const std::uint32_t *end) {
		std::vector<texture> tex_list;
		tex_list.reserve(count);
		for (std::uint32_t i = 0; i < count; ++i) {
			if (*ptr + 2 > end) break;
			unsigned int w = *(*ptr)++;
			unsigned int h = *(*ptr)++;

			std::vector<std::uint32_t> pixels(*ptr, *ptr + (w * h));
			tex_list.emplace_back(pixels, w, h);

			*ptr += (w * h);
		}
		return tex_list;
	};
}

