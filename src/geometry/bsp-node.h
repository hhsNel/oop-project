#pragma once
#define BSP_NODE_H

#include "math/vec2.h"
#include "util/indexed-storage.h"
#include "util/resource.h"
#include <vector>

namespace geometry {
	class bsp_node {
	public:
		struct bounding_box {
			float top, bottom, left, right;
		};

		math::vec2 pl_coord, pl_dir;
		bounding_box front_box, back_box;
		util::indexed_storage<bsp_node>::id_t front, back;
		static constexpr util::indexed_storage<bsp_node>::id_t leaf_flag = 1 << (sizeof(util::indexed_storage<bsp_node>::id_t) - 1);

		inline constexpr bool is_front_leaf() const;
		inline constexpr bool is_back_leaf() const;
		inline constexpr util::indexed_storage<bsp_node>::id_t get_front_index() const;
		inline constexpr util::indexed_storage<bsp_node>::id_t get_back_index() const;
		
		bool is_pt_front_side(math::vec2 const& point) const;

		static std::vector<bsp_node> const load_from_bin(util::resource const& res);
	};
}

inline constexpr bool geometry::bsp_node::is_front_leaf() const {
	return front & geometry::bsp_node::leaf_flag;
}

inline constexpr bool geometry::bsp_node::is_back_leaf() const {
	return back & geometry::bsp_node::leaf_flag;
}

inline constexpr util::indexed_storage<geometry::bsp_node>::id_t geometry::bsp_node::get_front_index() const {
	return front & ~geometry::bsp_node::leaf_flag;
}

inline constexpr util::indexed_storage<geometry::bsp_node>::id_t geometry::bsp_node::get_back_index() const {
	return back & ~geometry::bsp_node::leaf_flag;
}

