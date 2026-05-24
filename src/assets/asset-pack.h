#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <unordered_map>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"
#include "menu-element.h"
#include "menu.h"
#include "ids.h"

namespace assets {
	class asset_pack {
		std::vector<texture>	  wall_textures;
		std::vector<texture>	  sprite_textures;
		std::vector<texture>	  flat_textures;
		std::vector<texture>	  ui_textures;
		std::vector<menu>		 menus;

		asset_pack(std::vector<texture> walls, std::vector<texture> sprites, std::vector<texture> flats, std::vector<texture> ui, std::vector<menu> ms);

		static std::vector<texture>	  tx_from_meta(util::resource_loader &resld, std::string_view meta_path);
		static std::vector<menu>		 menus_from_meta(util::resource_loader &resld, std::string_view meta_path);
		static std::unordered_map<std::string, std::string> parse_meta_keys(std::string const& content);

	public:
		static asset_pack load(util::resource_loader &resld, std::string_view pack_meta_path);

		texture const& wall_tx_by_id(texture_id const id) const;
		texture const& sprite_tx_by_id(texture_id const id) const;
		texture const& flat_tx_by_id(texture_id const id) const;
		texture const& ui_tx_by_id(texture_id const id) const;
		menu &menu_by_id(menu_id const id);

		int display_menu(menu_id const id, rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend &in);
	};
}

