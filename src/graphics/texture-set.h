#pragma once
#define TEXTURE_SET_H

#include <vector>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"

namespace graphics {
	class texture_set {
		std::vector<texture> wall_textures;
		std::vector<texture> sprite_textures;
		std::vector<texture> flat_textures;

		texture_set(std::vector<texture> const& walls, std::vector<texture> const& sprites, std::vector<texture> const& flat);
		static std::vector<texture> tx_from_meta(util::resource_loader &resld, std::string_view meta_path);
	public:
		typedef std::uint32_t texture_id;

		static texture_set load(util::resource_loader &resld, std::string_view pack_meta_path);

		texture const& wall_tx_by_id(texture_id const id) const;
		texture const& sprite_tx_by_id(texture_id const id) const;
		texture const& flat_tx_by_id(texture_id const id) const;
	};
}

