#include "menu.h"

#include <sstream>

namespace assets {

	menu::menu(std::vector<menu_element> els, texture const *const b, texture const *const cur)
		: elements(els), bg(b), cursor(cur) {};

	menu const menu::load_from_bin(util::resource_loader &resld, util::resource const &res, std::vector<texture> const &ui_tx) {
		std::string content(reinterpret_cast<const char*>(res("beginning"_f)), res("size"_f));
		std::istringstream stream(content);
		std::size_t bg_tx_idx;
		texture const *bg_tx;
		std::string el_res_name;
		std::vector<menu_element> els;

		stream >> bg_tx_idx;
		bg_tx = (bg_tx_idx != static_cast<std::size_t>(-1)) ? &ui_tx[bg_tx_idx] : nullptr;

		while(stream >> el_res_name) {
			util::resource const *const r = resld.lookup_resource(el_res_name);
			if(r) {
				els.push_back(menu_element::load_from_bin(*r, ui_tx));
			}
		}

		// ui_tx[0] is always the cursor texture
		texture const *cursor_tx = ui_tx.empty() ? nullptr : &ui_tx[0];

		return menu(els, bg_tx, cursor_tx);
	}

	int menu::display(rendering::renderer_2d const& r2d, rendering::rendering_backend &rb, input::input_backend *in) const {
		in->reset_mouse_state();

		const int sw = static_cast<int>(rb("width"_f));
		const int sh = static_cast<int>(rb("height"_f));

		// Character dimensions for element label text
		constexpr int CHAR_W = 8;
		constexpr int CHAR_H = 16;
		constexpr std::uint32_t TEXT_COLOR = 0xFFFFFFFF;

		while (true) {
			in->update();

			// Draw menu background, or a plain black fallback
			if (bg) {
				r2d.draw_texture(*bg, 0, 0, sw, sh);
			} else {
				r2d.draw_rect(0, 0, sw, sh, 0xFF000000);
			}

			input::mouse_state ms = in->get_mouse_state();

			// Draw elements and test for a click
			for (menu_element const& el : elements) {
				int x0 = static_cast<int>(el.begin_x * sw);
				int y0 = static_cast<int>(el.begin_y * sh);
				int x1 = static_cast<int>(el.end_x   * sw);
				int y1 = static_cast<int>(el.end_y   * sh);
				int w  = x1 - x0;
				int h  = y1 - y0;

				// Element background texture, or a semi-transparent grey fallback
				if (el.bg) {
					r2d.draw_texture(*el.bg, x0, y0, w, h);
				} else {
					r2d.draw_rect(x0, y0, w, h, 0x80404040);
				}

				// Label: vertically centred, small left padding
				if (!el.str.empty()) {
					int text_x = x0 + 4;
					int text_y = y0 + (h - CHAR_H) / 2;
					r2d.draw_text(el.str, text_x, text_y, CHAR_W, CHAR_H, TEXT_COLOR);
				}

				// A left-click inside the element's bounds returns its function id
				if (ms.left
					&& ms.x >= x0 && ms.x < x1
					&& ms.y >= y0 && ms.y < y1)
				{
					return el.function;
				}
			}

			// Draw the cursor on top of everything (ui_tx[0])
			if (cursor) {
				const int cw = static_cast<int>(cursor->width);
				const int ch = static_cast<int>(cursor->height);
				r2d.draw_texture(*cursor, ms.x, ms.y, cw, ch);
			}

			// ESC cancels the menu; -1 signals "no selection"
			if (in->is_key_down(input::key::esc)) {
				return -1;
			}
		}
	}

};