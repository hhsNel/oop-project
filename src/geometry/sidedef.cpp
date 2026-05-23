#include "geometry/sidedef.h"

namespace geometry {
	struct bin_sidedef {
		std::uint32_t facing_sector;
		std::uint32_t upper_tex, middle_tex, lower_tex;
	} __attribute__((packed));

	sidedef::sidedef(util::indexed_storage<sector>::id_t sector, assets::texture_id ut, assets::texture_id mt, assets::texture_id lt) :
		facing_sector(sector),
		upper_tex(ut),
		middle_tex(mt),
		lower_tex(lt) {}

	std::vector<sidedef> sidedef::load_from_bin(util::resource const& res) {
		size_t count = res("size"_f) / sizeof(bin_sidedef);
		auto const* data = reinterpret_cast<bin_sidedef const*>(res("beginning"_f));

		std::vector<sidedef> result;
		result.reserve(count);

		for (size_t i = 0; i < count; ++i) {
			result.emplace_back(data[i].facing_sector, data[i].upper_tex, data[i].middle_tex, data[i].lower_tex);
		}
		return result;
	}
}
