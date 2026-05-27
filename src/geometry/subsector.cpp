#include "geometry/subsector.h"

#include <utility>
#include <algorithm>

namespace geometry {
	struct bin_subsector {
		std::uint32_t line_count;
		std::uint32_t first_line_id;
	} __attribute__((packed));

	subsector::subsector(std::vector<util::indexed_storage<linedef>::id_t> l) :
		lines(std::move(l)) {}

	std::vector<subsector> subsector::load_from_bin(util::resource const& res) {
		size_t count = res("size"_f) / sizeof(bin_subsector);
		auto const* data = reinterpret_cast<bin_subsector const*>(res("beginning"_f));

		std::vector<subsector> result;
		result.reserve(count);

		for (size_t i = 0; i < count; ++i) {
			std::vector<util::indexed_storage<linedef>::id_t> lines;
			lines.reserve(data[i].line_count);
			for (std::uint32_t j = 0; j < data[i].line_count; ++j) {
				// binary uses 0-based indices; indexed_storage IDs start at 1
				lines.push_back(data[i].first_line_id + j + 1);
			}
			result.emplace_back(std::move(lines));
		}
		return result;
	}

	void subsector::remove_sprite(rendering::sprite* spr) {
		auto it = std::find(sprites.begin(), sprites.end(), spr);
		if (it != sprites.end())
			sprites.erase(it);
	}

	void subsector::add_sprite(rendering::sprite* spr) {
		sprites.push_back(spr);
	}
}
