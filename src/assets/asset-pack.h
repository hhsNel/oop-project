#pragma once
#define ASSET_PACK_H

#include <vector>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"
#include "menu-element.h"

namespace assets {
	class asset_pack {
		std::vector<texture> wall_textures;
		std::vector<texture> sprite_textures;
		std::vector<texture> flat_textures;
		std::vector<texture> ui_backgrounds;
		std::vector<menu_element> menu_elements;

		asset_pack(
			std::vector<texture> const& walls,
			std::vector<texture> const& sprites,
			std::vector<texture> const& flats,
			std::vector<texture> const& ui,
			std::vector<menu_element> const& menu_elems
		);

		static std::vector<texture> tx_from_meta(util::resource_loader &resld, std::string_view meta_path);
		static std::vector<menu_element> menu_elems_from_meta(util::resource_loader &resld, std::string_view meta_path, std::vector<texture> const& ui_tx);
    static std::unordered_map<std::string, std::string> parse_meta_keys(std::string const& content);

	public:
		typedef std::uint32_t texture_id;
		typedef std::uint32_t element_id;

		static asset_pack load(util::resource_loader &resld, std::string_view pack_meta_path);

		texture      const& wall_tx_by_id(texture_id const id) const;
		texture      const& sprite_tx_by_id(texture_id const id) const;
		texture      const& flat_tx_by_id(texture_id const id) const;
		texture      const& ui_tx_by_id(texture_id const id) const;
		menu_element const& menu_elem_by_id(element_id const id) const;
	};
}
