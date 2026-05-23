#pragma once
#define LINEDEF_H

#include "math/segment.h"
#include "util/resource.h"
#include "util/indexed-storage.h"
#include "util/componentized.h"
#include <vector>

namespace rendering {
	class software_renderer;
}

namespace geometry {
	class sidedef;

	class linedef : public util::componentized<linedef> {
		[[=util::component_field{}]] math::segment seg;
		
		util::indexed_storage<sidedef>::id_t front;
		util::indexed_storage<sidedef>::id_t back;

	public:

		linedef(math::vec2 const p0, math::vec2 const p1, util::indexed_storage<sidedef>::id_t const f, util::indexed_storage<sidedef>::id_t const b);

		static std::vector<linedef> load_from_bin(util::resource const& res);

		__attribute__((always_inline)) constexpr inline bool is_wall() const;
		__attribute__((always_inline)) constexpr inline bool is_portal() const;
		__attribute__((always_inline)) constexpr inline math::vec2 dir() const;
		__attribute__((always_inline)) constexpr inline float len() const;

		friend util::componentized<linedef>;
		friend rendering::software_renderer;
	};
}

__attribute__((always_inline)) constexpr inline bool geometry::linedef::is_wall() const {
	return back == util::indexed_storage<sidedef>::nullid;
}

__attribute__((always_inline)) constexpr inline bool geometry::linedef::is_portal() const {
	return !is_wall();
}

__attribute__((always_inline)) constexpr inline math::vec2 geometry::linedef::dir() const {
	return (seg("point1"_f) - seg("point0"_f)).normalized();
}

__attribute__((always_inline)) constexpr inline float geometry::linedef::len() const {
	return seg.len();
}

