#pragma once
#define MENU_H

#include <vector>
#include <string>
#include <string_view>
#include <functional>

#include "util/resource.h"
#include "util/resource-loader.h"
#include "texture.h"
#include "menu-element.h"
#include "input/input-backend.h"
#include "ids.h"

namespace rendering {
	class rendering_backend;
	class renderer_2d;
}

namespace assets {
	class menu {
		menu(std::vector<menu_element> els, texture_id const b);

		std::vector<menu_element> elements;
		texture_id const bg;

	public:
		static menu load_from_bin(util::resource_loader &resld, util::resource const &res);

		/* usage: */
		/* menu.set_formatter_for("{name}", [&](std::string_view tmpl) { */
		/*     std::string s(tmpl); */
		/*     for (std::size_t p; (p = s.find("{name}")) != std::string::npos;) */
		/*         s.replace(p, 7, player_name); */
		/*     return s; */
		/* }); */
		void selective_formatter(std::string_view key_filter, std::function<std::string(std::string_view)> fn);

		void formatter_all(std::function<std::string(std::string_view)> fn);

		int display(rendering::renderer_2d const &r2d, rendering::rendering_backend &rb, input::input_backend &in, std::vector<texture> const& ui_tx) const;
	};
}
