#pragma once
#define ASSET_MANAGER_H

#include <vector>
#include <string>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "asset-pack.h"
#include "input/input-backend.h"

namespace rendering {
	class renderer_2d;
	class rendering_backend;
}

namespace assets {
	class asset_manager {
		util::resource_loader &rl;
		std::vector<asset_pack> asset_packs;
		unsigned int cur_set;

		asset_manager(util::resource_loader &resld, std::vector<asset_pack> packs);

	public:
		static asset_manager load(util::resource_loader &resld);

		void cycle_set();

		texture const& wall_tx_by_id(asset_pack::texture_id const id) const;
		texture const& sprite_tx_by_id(asset_pack::texture_id const id) const;
		texture const& flat_tx_by_id(asset_pack::texture_id const id) const;
		texture const& ui_tx_by_id(asset_pack::texture_id const id) const;
		menu &menu_by_id(asset_pack::menu_id const id);

		int display_menu(asset_pack::menu_id const id, rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend *in);
	};
}

