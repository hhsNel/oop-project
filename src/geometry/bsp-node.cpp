#include "bsp-node.h"

namespace geometry {
	struct bin_bsp_node {
		float pl_x, pl_y, dir_x, dir_y;
		float f_top, f_bot, f_left, f_right;
		float b_top, b_bot, b_left, b_right;
		std::uint32_t front_child, back_child;
	} __attribute__((packed));

	bool bsp_node::is_pt_front_side(math::vec2 const& point) const {
		math::vec2 delta = point - pl_coord;
		return math::vec2::cross_product(pl_dir, delta) <= 0;
	}

	bsp_node::bsp_node(math::vec2 const pc, math::vec2 const pd, bounding_box const fb, bounding_box const bb, util::indexed_storage<bsp_node>::id_t f, util::indexed_storage<bsp_node>::id_t b) :
		pl_coord(pc),
		pl_dir(pd),
		front_box(fb),
		back_box(bb),
		front(f),
		back(b) {}

	std::vector<bsp_node> const bsp_node::load_from_bin(util::resource const& res) {
		std::vector<bsp_node> result;
		if (!res("beginning"_f) || res("size"_f) == 0) return result;

		size_t count = res("size"_f) / sizeof(bin_bsp_node);
		auto const* data = reinterpret_cast<bin_bsp_node const*>(res("beginning"_f));

		for (size_t i = 0; i < count; ++i) {
			bounding_box front_box = {
				float(data[i].f_top), float(data[i].f_bot),
				float(data[i].f_left), float(data[i].f_right)
			};
			bounding_box back_box = {
				float(data[i].b_top), float(data[i].b_bot),
				float(data[i].b_left), float(data[i].b_right)
			};

			math::vec2 pl_coord(float(data[i].pl_x), float(data[i].pl_y));
			math::vec2 pl_dir(float(data[i].dir_x), float(data[i].dir_y));

			// binary uses 0-based indices; indexed_storage IDs start at 1
			auto fix_child = [](std::uint32_t raw) -> std::uint32_t {
				if (raw & leaf_flag)
					return leaf_flag | ((raw & ~leaf_flag) + 1);
				return raw + 1;
			};

			result.emplace_back(pl_coord, pl_dir, front_box, back_box, fix_child(data[i].front_child), fix_child(data[i].back_child));
		}
		return result;
	}
}
