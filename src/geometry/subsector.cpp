#include "geometry/subsector.h"

#include <utility>

namespace geometry {
	struct bin_subsector {
		std::uint32_t line_count;
		std::uint32_t first_line_id;
	} __attribute__((packed));

	subsector::subsector(std::vector<util::indexed_storage<linedef>::id_t> l, std::vector<std::unique_ptr<rendering::sprite>> s) :
		lines(std::move(l)),
		sprites(std::move(s)) {}

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
			result.emplace_back(std::move(lines), std::vector<std::unique_ptr<rendering::sprite>>{});
		}
		return result;
	}

	std::unique_ptr<rendering::sprite> subsector::remove_sprite(rendering::sprite* spr) {
		auto it = std::find_if(sprites.begin(), sprites.end(),
			[spr](std::unique_ptr<rendering::sprite> const& p) {
				return p.get() == spr;
			});

		if (it == sprites.end()) return nullptr;

		auto owned = std::move(*it);
		sprites.erase(it);
		return owned;
	}

	void subsector::add_sprite(std::unique_ptr<rendering::sprite> spr) {
		sprites.push_back(std::move(spr));
	}
}
