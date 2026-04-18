#pragma once
#define TEXTURE_MANAGER_H

#include <vector>
#include <cstddef>
#include <cstdint>

#include "util/resource.h"
#include "texture.h"

namespace graphics {
	class texture_manager {
		std::vector<texture> sprite_textures;
		std::vector<texture> flat_textures;

		texture_manager(std::vector<texture> const& sprites, std::vector<texture> const& flat);

		static std::vector<texture> read_textures(std::uint32_t count, const std::uint32_t **ptr, const std::uint32_t *end);
	public:
		typedef std::uint32_t texture_id;

		static texture_manager const load_from_bin(util::resource const res);

		texture const& sprite_tx_by_id(texture_id const id) const;
		texture const& flat_tx_by_id(texture_id const id) const;
	};
}

