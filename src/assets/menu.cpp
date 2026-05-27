#include "menu.h"
#include "rendering/renderer-2d.h"
#include "rendering/rendering-backend.h"

#include <sstream>

namespace assets {

	menu::menu(std::vector<menu_element> els, texture_id const b) :
		elements(std::move(els)),
		bg(b) {}

	/* bin fmt */
	/* <bg_tx_idx> */
	/* <element_resource_name> ... */
	menu menu::load_from_bin(util::resource_loader &resld, util::resource const &res) {
		std::string content(reinterpret_cast<const char *>(res("beginning"_f)), res("size"_f));
		std::istringstream stream(content);

		int bg_tx_idx;
		stream >> bg_tx_idx;

		std::vector<menu_element> els;
		std::string el_res_name;
		while (stream >> el_res_name) {
			util::resource const *const r = resld.lookup_resource(el_res_name);
			if (r) {
				els.push_back(menu_element::load_from_bin(*r));
			}
		}

		return menu(std::move(els), bg_tx_idx);
	}

	void menu::selective_formatter(std::string_view key_filter, std::function<std::string(std::string_view)> fn) {
		for (menu_element &el : elements) {
			el.selective_fmt(key_filter, fn);
		}
	}

	void menu::formatter_all(std::function<std::string(std::string_view)> fn) {
		selective_formatter("", fn);
	}

	int menu::display(rendering::renderer_2d const &r2d, rendering::rendering_backend &rb, input::input_backend &in, std::vector<texture> const& ui_tx) const {
		const int sw = static_cast<int>(rb("width"_f));
		const int sh = static_cast<int>(rb("height"_f));

		in.reset_mouse_state(sw / 2, sh / 2);

		bool esc_debounce = in.is_key_down(input::key::esc);

		while (true) {
			in.update();

			if (bg >= 0) {
				r2d.draw_texture(ui_tx[bg], 0, 0, sw, sh);
			} else {
				r2d.draw_rect(0, 0, sw, sh, 0xFF000000);
			}

			input::mouse_state const ms = in.get_mouse_state();

			for (menu_element const &el : elements) {
				int result = el.render(r2d, ui_tx, sw, sh, ms);
				if (result != -2) {
					return result;
				}
			}

			const int cw = static_cast<int>(ui_tx[1]("width"_f));
			const int ch = static_cast<int>(ui_tx[1]("height"_f));
			r2d.draw_texture(ui_tx[1], ms.x, ms.y, cw, ch);

			if (esc_debounce) {
				if (!in.is_key_down(input::key::esc)) esc_debounce = false;
			} else if (in.is_key_down(input::key::esc)) {
				return -1;
			}

			rb.flush();
		}
	}
}

