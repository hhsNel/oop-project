#pragma once

#include "assets/ids.h"
#include "util/resource.h"
#include "util/indexed-storage.h"
#include <vector>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class sector;

	class sidedef {
		/* sector the sidedef is facing */
		util::indexed_storage<sector>::id_t facing_sector;
		/* portal upper texture */
		assets::texture_id upper_tex;
		/* wall texture */
		assets::texture_id middle_tex;
		/* portal lower texture */
		assets::texture_id lower_tex;

	public:

		/* constructor */
		sidedef(util::indexed_storage<sector>::id_t sector, assets::texture_id ut, assets::texture_id mt, assets::texture_id lt);

		/* load subsectors from a binary */
		static std::vector<sidedef> load_from_bin(util::resource const& res);

		friend rendering::software_renderer;
	};
}
