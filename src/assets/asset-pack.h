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
#include "audio-clip.h"
#include "ids.h"

namespace assets {
	class asset_pack {
		/* all textures */
		std::vector<texture> wall_textures;
		/* all textures */
		std::vector<texture> sprite_textures;
		/* all textures */
		std::vector<texture> flat_textures;
		/* all textures */
		std::vector<texture> ui_textures;
		/* all menus */
		std::vector<menu> menus;
		/* all audio clips */
		std::vector<audio_clip> audio_clips;

		/* constructor */
		asset_pack(std::vector<texture> walls, std::vector<texture> sprites,
			std::vector<texture> flats, std::vector<texture> ui,
			std::vector<menu> ms, std::vector<audio_clip> clips);

		/* load texures from a meta file */
		static std::vector<texture> tx_from_meta(util::resource_loader &resld,
			std::string_view meta_path);
		/* load menus from a meta file */
		static std::vector<menu> menus_from_meta(util::resource_loader &resld,
			std::string_view meta_path);
		/* load audio clips from a meta file */
		static std::vector<audio_clip> clips_from_meta(util::resource_loader &resld,
			std::string_view meta_path);
		/* parse keys in ap delaration meta file */
		static std::unordered_map<std::string, std::string>
			parse_meta_keys(std::string const& content);

	public:
		/* load from a binary */
		static asset_pack load(util::resource_loader &resld,
			std::string_view pack_meta_path);

		/* lookup textures */
		texture const& wall_tx_by_id(texture_id const id) const;
		/* lookup textures */
		texture const& sprite_tx_by_id(texture_id const id) const;
		/* lookup textures */
		texture const& flat_tx_by_id(texture_id const id) const;
		/* lookup textures */
		texture const& ui_tx_by_id(texture_id const id) const;
		/* lookup a menu */
		menu &menu_by_id(menu_id const id);
		/* lookup an audio clip */
		audio_clip const& audio_clip_by_id(audio_clip_id const id) const;

		/* display a menu */
		int display_menu(menu_id const id, rendering::renderer_2d const& r2d,
			rendering::rendering_backend &rb, input::input_backend &in);
	};
}

