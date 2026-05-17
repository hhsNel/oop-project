#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <unordered_map>
#include <string>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "asset-pack.h"

namespace assets {
	class texture_manager {
		util::resource_loader &rl;
		std::vector<asset_pack> asset_packs;
		unsigned int cur_set;

		texture_manager(util::resource_loader &resld, std::vector<asset_pack> packs);
	public:
		static texture_manager load(util::resource_loader &resld);

		void cycle_set();

		texture const& wall_tx_by_id(asset_pack::texture_id const id) const;
		texture const& sprite_tx_by_id(asset_pack::texture_id const id) const;
		texture const& flat_tx_by_id(asset_pack::texture_id const id) const;
	};
}

#endif
