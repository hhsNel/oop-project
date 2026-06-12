#pragma once

#include "util/resource.h"
#include "util/indexed-storage.h"
#include "rendering/sprite.h"
#include <vector>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class linedef;

	class subsector {
		/* lines belonging to a subsector */
		std::vector<util::indexed_storage<linedef>::id_t> lines;
		/* sprites belonging to a subsector */
		std::vector<rendering::sprite*> sprites;

	public:

		/* constructor */
		subsector() = default;
		/* constructor */
		subsector(std::vector<util::indexed_storage<linedef>::id_t> l);

		/* load subsectors from a binary */
		static std::vector<subsector> load_from_bin(util::resource const& res);

		/* remove a sprite from a subsector */
		void remove_sprite(rendering::sprite* spr);
		/* add a sprite to a subsector */
		void add_sprite(rendering::sprite* spr);

		friend rendering::software_renderer;
	};
}
