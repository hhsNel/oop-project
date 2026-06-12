#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "asset-pack.h"
#include "input/input-backend.h"
#include "ids.h"

namespace rendering {
	class renderer_2d;
	class rendering_backend;
}

namespace assets {
	class asset_manager {
		/* resource loader reference */
		util::resource_loader &rl;
		/* all loaded asset packs */
		std::vector<asset_pack> asset_packs;
		/* current asset pack */
		unsigned int cur_set;

		/* constructor */
		asset_manager(util::resource_loader &resld, std::vector<asset_pack> packs);

	public:
		/* load all assets */
		static asset_manager load(util::resource_loader &resld);

		/* next asset pack */
		void cycle_set();

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

