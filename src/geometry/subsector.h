#pragma once
#define SUBSECTOR_H

#include "util/resource.h"
#include "util/indexed-storage.h"
#include "rendering/sprite.h"
#include <vector>
#include <memory>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class linedef;

	class subsector {
		std::vector<util::indexed_storage<linedef>::id_t> lines;
		std::vector<std::unique_ptr<rendering::sprite>> sprites;

	public:

		subsector() = default;
		subsector(std::vector<util::indexed_storage<linedef>::id_t> l, std::vector<std::unique_ptr<rendering::sprite>> s);

		static std::vector<subsector> load_from_bin(util::resource const& res);

		std::unique_ptr<rendering::sprite> remove_sprite(rendering::sprite* spr);
		void add_sprite(std::unique_ptr<rendering::sprite> spr);

		friend rendering::software_renderer;
	};
}
