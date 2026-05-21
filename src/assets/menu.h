#pragma once
#define MENU_H

#include <vector>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"
#include "menu-element.h"
#include "input/input-backend.h"

namespace rendering {
	class rendering_backend;
	class renderer_2d;
}

namespace assets {
	class menu {
		menu(std::vector<menu_element> els, texture const *const b, texture const *const cur);

		std::vector<menu_element> elements;
		texture const *const bg;
		texture const *const cursor;

	public:

		static menu const load_from_bin(util::resource_loader &resld, util::resource const &res, std::vector<texture> const &ui_tx);

		int display(rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend *in) const;
	};
}
