#include "menu-element.h"
#include "rendering/renderer-2d.h"
#include "input/input-backend.h"

#include <sstream>
#include <limits>

namespace assets {

	menu_element::menu_element(int f, float bx, float by, float ex, float ey, texture_id const b, std::string s, int cw, int ch, std::uint8_t r, std::uint8_t g, std::uint8_t b_col, float rlm) :
		function(f),
		begin_x(bx),
		begin_y(by),
		end_x(ex),
		end_y(ey),
		bg(b),
		fmt_template(std::move(s)),
		char_w(cw),
		char_h(ch),
		col_r(r),
		col_g(g),
		col_b(b_col),
		relative_left_margin(rlm) {}

	/* bin fmt: */
	/* <function> <bx> <by> <ex> <ey> <bg_tx_idx> <char_w> <char_h> <r> <g> <b> <relative_left_margin> */
	/* <format template string — rest of next line> */
	menu_element menu_element::load_from_bin(util::resource const &res) {
		std::string content(reinterpret_cast<const char *>(res("beginning"_f)), res("size"_f));
		std::istringstream stream(content);

		int f;
		float bx, by, ex, ey;
		int bg_tx_idx;
		int cw, ch;
		int r, g, b;
		float rlm;
		std::string tmpl;

		stream >> f >> bx >> by >> ex >> ey >> bg_tx_idx
			   >> cw >> ch >> r >> g >> b >> rlm;

		stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(stream, tmpl);

		return menu_element(f, bx, by, ex, ey, bg_tx_idx, std::move(tmpl), cw, ch, static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), rlm);
	}

	std::string menu_element::resolve_text() const {
		if (formatter) {
			return formatter(fmt_template);
		}
		return fmt_template;
	}

	std::uint32_t menu_element::text_color() const {
		return (0xFFu << 24)
			 | (static_cast<std::uint32_t>(col_r) << 16)
			 | (static_cast<std::uint32_t>(col_g) <<  8)
			 |  static_cast<std::uint32_t>(col_b);
	}

	void menu_element::selective_fmt(std::string_view key_filter, std::function<std::string(std::string_view)> fn) {
		if (key_filter.empty()) {
			formatter = fn;
			return;
		}
		std::string filter(key_filter);
		std::istringstream stream(filter);
		std::string token;
		while (std::getline(stream, token, '|')) {
			if (!token.empty() && fmt_template.find(token) != std::string::npos) {
				formatter = fn;
				return;
			}
		}
	}

	int menu_element::render(rendering::renderer_2d const &r2d, std::vector<texture> const& ui_tx, int sw, int sh, input::mouse_state const &ms) const {
		int x0 = static_cast<int>(begin_x * sw);
		int y0 = static_cast<int>(begin_y * sh);
		int x1 = static_cast<int>(end_x   * sw);
		int y1 = static_cast<int>(end_y   * sh);
		int w  = x1 - x0;
		int h  = y1 - y0;

		if (bg >= 0) {
			r2d.draw_texture(ui_tx[bg], x0, y0, w, h);
		} else {
			r2d.draw_rect(x0, y0, w, h, 0x80404040);
		}

		std::string label = resolve_text();
		if (!label.empty()) {
			int text_x = x0 + static_cast<int>(relative_left_margin * w);
			int text_y = y0 + (h - char_h) / 2;
			r2d.draw_text(label, text_x, text_y, char_w, char_h, text_color());
		}

		if (ms.left && ms.x >= x0 && ms.x < x1 && ms.y >= y0 && ms.y < y1) {
			return function;
		}

		return -2;
	}
}
