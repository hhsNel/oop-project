#pragma once
#define MENU_H

#include <vector>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"
#include "menu-element.h"
#include "rendering/renderer-2d.h"
#include "rendering/rendering-backend.h"
#include "input/input-backend.h"

namespace assets {
	class menu {
		menu(std::vector<menu_element> els, texture const *const b, texture const *const cur);

		std::vector<menu_element> elements;
		texture const *const bg;
		texture const *const cursor; // always ui_tx[0]

	public:

		static menu const load_from_bin(util::resource_loader &resld, util::resource const &res, std::vector<texture> const &ui_tx);

		// Runs the menu loop. Draws the menu each frame, returns the function id
		// of the clicked element, or -1 if ESC was pressed.
		int display(rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend *in) const;
	};
}