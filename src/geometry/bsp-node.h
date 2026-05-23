#pragma once
#define BSP_NODE_H

#include "math/vec2.h"
#include "util/indexed-storage.h"
#include "util/resource.h"
#include <vector>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class bsp_node {
	public:
		struct bounding_box {
			float top, bottom, left, right;
		};
	
	private:

		math::vec2 pl_coord, pl_dir;
		bounding_box front_box, back_box;
		util::indexed_storage<bsp_node>::id_t front, back;
		static constexpr util::indexed_storage<bsp_node>::id_t leaf_flag = 1 << (sizeof(util::indexed_storage<bsp_node>::id_t) * 8 - 1);
	
	public:

		__attribute__((always_inline)) inline constexpr bool is_front_leaf() const;
		__attribute__((always_inline)) inline constexpr bool is_back_leaf() const;
		__attribute__((always_inline)) inline constexpr util::indexed_storage<bsp_node>::id_t get_front_index() const;
		__attribute__((always_inline)) inline constexpr util::indexed_storage<bsp_node>::id_t get_back_index() const;
		
		bool is_pt_front_side(math::vec2 const& point) const;

		bsp_node(math::vec2 const pc, math::vec2 const pd, bounding_box const fb, bounding_box const bb, util::indexed_storage<bsp_node>::id_t f, util::indexed_storage<bsp_node>::id_t b);
		static std::vector<bsp_node> const load_from_bin(util::resource const& res);

		__attribute__((always_inline)) static inline constexpr bool is_leaf(util::indexed_storage<bsp_node>::id_t const id);
		__attribute__((always_inline)) static inline constexpr util::indexed_storage<bsp_node>::id_t get_id(util::indexed_storage<bsp_node>::id_t const raw);

		friend rendering::software_renderer;
	};
}

__attribute__((always_inline)) inline constexpr bool geometry::bsp_node::is_front_leaf() const {
	return is_leaf(front);
}

__attribute__((always_inline)) inline constexpr bool geometry::bsp_node::is_back_leaf() const {
	return is_leaf(back);
}

__attribute__((always_inline)) inline constexpr util::indexed_storage<geometry::bsp_node>::id_t geometry::bsp_node::get_front_index() const {
	return get_id(front);
}

__attribute__((always_inline)) inline constexpr util::indexed_storage<geometry::bsp_node>::id_t geometry::bsp_node::get_back_index() const {
	return get_id(back);
}

__attribute__((always_inline)) inline constexpr bool geometry::bsp_node::is_leaf(util::indexed_storage<geometry::bsp_node>::id_t const id) {
	return id & geometry::bsp_node::leaf_flag;
}

__attribute__((always_inline)) inline constexpr util::indexed_storage<geometry::bsp_node>::id_t geometry::bsp_node::get_id(util::indexed_storage<geometry::bsp_node>::id_t const raw) {
	return raw & ~geometry::bsp_node::leaf_flag;
}

