#include "geometry/sector.h"
#include <cstring>

namespace geometry {
	struct bin_sector {
		float floor_height, ceiling_height;
		std::uint32_t floor_tex, ceiling_tex;
		std::uint8_t light_level;
	} __attribute__((packed));

	sector::sector(float fh, float ch, assets::texture_id ft, assets::texture_id ct, std::uint8_t light) :
		floor_height(fh),
		ceiling_height(ch),
		floor_tex(ft),
		ceiling_tex(ct),
		light_level(light) {}

	std::vector<sector> sector::load_from_bin(util::resource const& res) {
		if (!res("beginning"_f) || res("size"_f) == 0) return {};

		size_t count = res("size"_f) / sizeof(bin_sector);
		auto const* data = reinterpret_cast<bin_sector const*>(res("beginning"_f));

		std::vector<sector> result;
		result.reserve(count);

		for (size_t i = 0; i < count; ++i) {
			result.emplace_back(data[i].floor_height, data[i].ceiling_height, data[i].floor_tex, data[i].ceiling_tex, data[i].light_level);
		}
		return result;
	}
}
