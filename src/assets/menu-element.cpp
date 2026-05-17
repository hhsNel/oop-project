#include "menu-element.h"

#include <sstream>

namespace assets {

	menu_element::menu_element(float const bx, float const by, float const ex, float const ey, texture const *const b, std::string const s)
		: begin_x(bx), begin_y(by), end_x(ex), end_y(ey), bg(b), str(s) {};

	menu_element const menu_element::load_from_bin(util::resource const &res, std::vector<texture> const &ui_tx) {
		std::string content(reinterpret_cast<const char*>(res("beginning"_f)), res("size"_f));
		std::istringstream stream(content);
		float bx, by, ex, ey;
		std::size_t bg_tx_idx;
		texture const *bg_tx;
		std::string s;

		stream >> bx >> by >> ex >> ey >> bg_tx_idx;
		bg_tx = (bg_tx_idx != static_cast<std::size_t>(-1)) ? &ui_tx[bg_tx_idx] : nullptr;

		stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(stream, s);

		return menu_element(bx, by, ex, ey, bg_tx, s);
	}
}