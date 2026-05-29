#include "geometry/monster-spawn.h"

namespace geometry {
	monster_spawn::monster_spawn(std::uint32_t t, math::vec2 p, float height)
		: type(t), pos(p), z(height) {}

	class bin_monster_spawn {
	public:
		std::uint32_t type;
		int x, y;
		float z;
	} __attribute__((packed));

	std::vector<monster_spawn> monster_spawn::load_from_bin(util::resource const& res) {
		if (!res("beginning"_f) || res("size"_f) == 0) return {};

		std::vector<monster_spawn> result;
		size_t count = res("size"_f) / sizeof(bin_monster_spawn);
		auto const* data = reinterpret_cast<bin_monster_spawn const*>(res("beginning"_f));

		for (size_t i = 0; i < count; ++i) {
			result.emplace_back(data[i].type, math::vec2(float(data[i].x), float(data[i].y)), data[i].z);
		}
		return result;
	}
}
