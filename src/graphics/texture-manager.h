#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <unordered_map>
#include <string>
#include <string_view>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture-set.h"

namespace graphics {
	class texture_manager {
		util::resource_loader &rl;
		std::vector<texture_set> texture_sets;
		unsigned int cur_set;

		texture_manager(util::resource_loader &resld, std::vector<texture_set> packs);
	public:
		static texture_manager load(util::resource_loader &resld);

		void cycle_set();

		texture const& wall_tx_by_id(texture_set::texture_id const id) const;
		texture const& sprite_tx_by_id(texture_set::texture_id const id) const;
		texture const& flat_tx_by_id(texture_set::texture_id const id) const;
	};
}

#endif
