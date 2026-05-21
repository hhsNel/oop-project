#include "menu.h"
#include "rendering/renderer-2d.h"
#include "rendering/rendering-backend.h"

#include <sstream>

namespace assets {

	menu::menu(std::vector<menu_element> els, texture const *const b, texture const *const cur) :
		elements(std::move(els)),
		bg(b),
		cursor(cur) {}

	/* bin fmt */
	/* <bg_tx_idx> */
	/* <element_resource_name> ... */
	menu menu::load_from_bin(util::resource_loader &resld, util::resource const &res, std::vector<texture> const &ui_tx) {
		std::string content(reinterpret_cast<const char *>(res("beginning"_f)), res("size"_f));
		std::istringstream stream(content);

		int bg_tx_idx;
		stream >> bg_tx_idx;
		texture const *bg_tx = (bg_tx_idx >= 0) ? &ui_tx[bg_tx_idx] : nullptr;

		std::vector<menu_element> els;
		std::string el_res_name;
		while (stream >> el_res_name) {
			util::resource const *const r = resld.lookup_resource(el_res_name);
			if (r) {
				els.push_back(menu_element::load_from_bin(*r, ui_tx));
			}
		}

		texture const *cursor_tx = ui_tx.empty() ? nullptr : &ui_tx[0];
		return menu(std::move(els), bg_tx, cursor_tx);
	}

	void menu::selective_formatter(std::string_view key_filter, std::function<std::string(std::string_view)> fn) {
		for (menu_element &el : elements) {
			el.selective_fmt(key_filter, fn);
		}
	}

	void menu::formatter_all(std::function<std::string(std::string_view)> fn) {
		selective_formatter("", fn);
	}

	int menu::display(rendering::renderer_2d const &r2d, rendering::rendering_backend &rb, input::input_backend *in) const {
		in->reset_mouse_state();

		const int sw = static_cast<int>(rb("width"_f));
		const int sh = static_cast<int>(rb("height"_f));

		while (true) {
			in->update();

			if (bg) {
				r2d.draw_texture(*bg, 0, 0, sw, sh);
			} else {
				r2d.draw_rect(0, 0, sw, sh, 0xFF000000);
			}

			input::mouse_state const ms = in->get_mouse_state();

			for (menu_element const &el : elements) {
				int result = el.render(r2d, sw, sh, ms);
				if (result != -2) {
					return result;
				}
			}

			if (cursor) {
				const int cw = static_cast<int>(cursor->width);
				const int ch = static_cast<int>(cursor->height);
				r2d.draw_texture(*cursor, ms.x, ms.y, cw, ch);
			}

			if (in->is_key_down(input::key::esc)) {
				return -1;
			}

			rb.flush();
		}
	}
}

