#pragma once

#include "util/resource.h"
#include "util/indexed-storage.h"
#include "rendering/sprite.h"
#include <vector>

namespace rendering {
	class software_renderer;
}

namespace game { class game; }

namespace geometry {
	class linedef;

	class subsector {
		std::vector<util::indexed_storage<linedef>::id_t> lines;
		std::vector<rendering::sprite*> sprites;

	public:

		subsector() = default;
		subsector(std::vector<util::indexed_storage<linedef>::id_t> l);

		static std::vector<subsector> load_from_bin(util::resource const& res);

		void remove_sprite(rendering::sprite* spr);
		void add_sprite(rendering::sprite* spr);

		friend rendering::software_renderer;
		friend class game::game;
	};
}
