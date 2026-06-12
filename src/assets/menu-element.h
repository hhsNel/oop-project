#pragma once

#include <vector>
#include <string>
#include <functional>

#include "util/componentized.h"
#include "util/resource.h"
#include "texture.h"
#include "ids.h"

namespace rendering {
	class renderer_2d;
}

namespace input {
	struct mouse_state;
}

namespace assets {
	class menu_element {
		/* constructor */
		menu_element(int f, float bx, float by, float ex, float ey,
			texture_id const b, std::string s, int cw, int ch, std::uint8_t r,
			std::uint8_t g, std::uint8_t b_col, float rlm);

		/* function within a menu */
		int function;
		/* dimentions relative to screen size */
		float const begin_x, begin_y, end_x, end_y;
		/* element background */
		texture_id const bg;
		/* element text */
		std::string fmt_template;
		/* set formatter */
		std::function<std::string(std::string_view)> formatter;
		/* character dimentions in pixels */
		int char_w, char_h;
		/* text color */
		std::uint8_t col_r, col_g, col_b;
		/* left margin, relative to element width */
		float relative_left_margin;

		/* run template through formatter */
		std::string resolve_text() const;
		/* pack r, g, b into color */
		std::uint32_t text_color() const;
		/* set a formatter */
		void selective_fmt(std::string_view key_filter,
			std::function<std::string(std::string_view)> fn);
		/* render an element */
		int render(rendering::renderer_2d const &r2d, std::vector<texture> const& ui_tx,
				int sw, int sh, input::mouse_state const &ms) const;

	public:
		/* load from a binary */
		static menu_element load_from_bin(util::resource const &res);

		friend class menu;
	};
}
