#pragma once
#define TEXTURE_MANAGER_H

#include <vector>
#include <stddef>

#include "util/resource.h"
#include "texture.h"

namespace graphics {
	class texture_manager {
		std::vector<texture> wall_textures;
		std::vector<texture> sprite_textures;
		std::vector<texture> flat_textures;


		texture_manager(std::vector<texture> walls, std::vector<texture> sprites, std::vector<texture> flat);
	public:
		typedef uint32_t texture_id;

		static texture const load_from_bin(util::resource const res);

		texture const& wall_tx_by_id(texture_id const id) const;
		texture const& sprite_tx_by_id(texture_id const id) const;
		texture const& flat_tx_by_id(texture_id const id) const;
	};
}

