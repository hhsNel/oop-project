#include "geometry/linedef.h"

namespace geometry {
	struct bin_linedef {
		int v1_x, v1_y, v2_x, v2_y;
		std::uint32_t front, back;
	} __attribute__((packed));

	linedef::linedef(math::vec2 const p0, math::vec2 const p1, util::indexed_storage<sidedef>::id_t const f, util::indexed_storage<sidedef>::id_t const b) :
		seg(p0, p1),
		front(f),
		back(b) {}

	std::vector<linedef> linedef::load_from_bin(util::resource const& res) {
		std::vector<linedef> result;
		size_t count = res("size"_f) / sizeof(bin_linedef);
		auto const* data = reinterpret_cast<bin_linedef const*>(res("beginning"_f));

		for (size_t i = 0; i < count; ++i) {
			result.emplace_back(math::vec2(float(data[i].v1_x), float(data[i].v1_y)), math::vec2(float(data[i].v2_x), float(data[i].v2_y)), data[i].front, data[i].back);
		}
		return result;
	}
}
