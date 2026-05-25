#pragma once
#define MENU_ELEMENT_H

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
		menu_element(int f, float bx, float by, float ex, float ey, texture_id const b, std::string s, int cw, int ch, std::uint8_t r, std::uint8_t g, std::uint8_t b_col, float rlm = 0.0f);

		int function;
		float const begin_x, begin_y, end_x, end_y;
		texture_id const bg;
		std::string fmt_template;
		std::function<std::string(std::string_view)> formatter;
		int char_w, char_h;
		std::uint8_t col_r, col_g, col_b;
		float relative_left_margin;

		std::string resolve_text() const;
		std::uint32_t text_color() const;
		void selective_fmt(std::string_view key_filter, std::function<std::string(std::string_view)> fn);
		int render(rendering::renderer_2d const &r2d, std::vector<texture> const& ui_tx, int sw, int sh, input::mouse_state const &ms) const;

	public:
		static menu_element load_from_bin(util::resource const &res);

		friend class menu;
	};
}