#include "geometry/pickup-spawn.h"

namespace geometry {
	class bin_pickup_spawn {
	public:
		std::uint32_t type;
		std::uint32_t subtype;
		int x, y;
		float z;
	} __attribute__((packed));

	std::vector<pickup_spawn> pickup_spawn::load_from_bin(util::resource const& res) {
		if (!res("beginning"_f) || res("size"_f) == 0) return {};

		std::vector<pickup_spawn> result;
		size_t count = res("size"_f) / sizeof(bin_pickup_spawn);
		auto const* data = reinterpret_cast<bin_pickup_spawn const*>(res("beginning"_f));

		for (size_t i = 0; i < count; ++i) {
			result.emplace_back(data[i].type, data[i].subtype, math::vec2(float(data[i].x), float(data[i].y)), data[i].z);
		}
		return result;
	}
}
