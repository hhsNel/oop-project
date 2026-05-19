#pragma once
#define MENU_ELEMENT_H

#include <vector>

#include "util/resource.h"
#include "texture.h"

namespace assets {
	class menu_element {
		menu_element(int f, float const bx, float const by, float const ex, float const ey, texture const *const b, std::string const s);

		int function;
		float const begin_x, begin_y, end_x, end_y;
		texture const *const bg;
		std::string const str;

		friend class menu;

	public:
		static menu_element const load_from_bin(util::resource const &res, std::vector<texture> const &ui_tx);
	};
}