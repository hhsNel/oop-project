#pragma once

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
		/* BSP node bounding box */
		struct bounding_box {
			float top, bottom, left, right;
		};
	
	private:

		/* partitioning line definition */
		math::vec2 pl_coord, pl_dir;
		/* front/back node bounding boxes */
		bounding_box front_box, back_box;
		/* front/back node ids */
		util::indexed_storage<bsp_node>::id_t front, back;
		/* leaf (subsector) bitmask */
		static constexpr util::indexed_storage<bsp_node>::id_t leaf_flag
			= 1 << (sizeof(util::indexed_storage<bsp_node>::id_t) * 8 - 1);
	
	public:

		/* is the front node a subsector */
		__attribute__((always_inline)) inline constexpr bool is_front_leaf() const;
		/* is the back node a subsector */
		__attribute__((always_inline)) inline constexpr bool is_back_leaf() const;
		/* the id of the front node */
		__attribute__((always_inline)) inline constexpr
			util::indexed_storage<bsp_node>::id_t get_front_index() const;
		/* the id of the back node */
		__attribute__((always_inline)) inline constexpr
			util::indexed_storage<bsp_node>::id_t get_back_index() const;
		
		/* on which side of the partitioning line does a point lie */
		bool is_pt_front_side(math::vec2 const& point) const;

		/* constructor */
		bsp_node(math::vec2 const pc, math::vec2 const pd, bounding_box const fb,
			bounding_box const bb, util::indexed_storage<bsp_node>::id_t f,
			util::indexed_storage<bsp_node>::id_t b);
		/* load subsectors from a binary */
		static std::vector<bsp_node> const load_from_bin(util::resource const& res);

		/* is any id a subsector */
		__attribute__((always_inline)) static inline constexpr bool
			is_leaf(util::indexed_storage<bsp_node>::id_t const id);
		/* literal id of a node from a possibly bitmasked one */
		__attribute__((always_inline)) static inline constexpr
			util::indexed_storage<bsp_node>::id_t
			get_id(util::indexed_storage<bsp_node>::id_t const raw);

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

