#pragma once


#include "sector.h"
#include "sidedef.h"
#include "linedef.h"
#include "subsector.h"
#include "bsp-node.h"
#include "monster-spawn.h"
#include "pickup-spawn.h"
#include "rendering/sprite.h"
#include "util/indexed-storage.h"
#include "util/componentized.h"

namespace rendering {
	class software_renderer;
}
namespace game {
	class game;
}

namespace geometry {
	class map_data : public util::componentized<map_data> {
		/* all sectors */
		util::indexed_storage<sector> sectors;
		/* all sidedefs */
		util::indexed_storage<sidedef> sidedefs;
		/* all linedefs */
		[[=util::component_field{}]] util::indexed_storage<linedef> linedefs;
		/* all subsectors */
		[[=util::ref_component_field{}]] util::indexed_storage<subsector> subsectors;
		/* BSP tree */
		util::indexed_storage<bsp_node> nodes;
		/* all monster spawns */
		std::vector<monster_spawn> monster_spawns;
		/* all pickup spawns */
		std::vector<pickup_spawn> pickup_spawns;

		/* root BSP node */
		[[=util::component_field{}]] util::indexed_storage<bsp_node>::id_t root_node_id;

	public:

		/* subsector from point */
		util::indexed_storage<subsector>::id_t get_subsector_id(math::vec2 const& pt) const;

		/* move sprite to a new position */
		void move_to(rendering::sprite* spr, math::vec2 const& new_pos);

		/* load from binaries */
		static map_data load_from_bin(util::resource const& sectors_res,
			util::resource const& sidedefs_res, util::resource const& linedefs_res,
			util::resource const& subsectors_res, util::resource const& nodes_res,
			util::resource const& monsters_res, util::resource const& pickups_res);

		friend util::componentized<map_data>;
		friend rendering::software_renderer;
		friend game::game;
	};
}

