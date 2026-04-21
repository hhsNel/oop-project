#pragma once

#include <cstdint>

namespace input {
	enum class key {
		unknown = 0,
		a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
		n0, n1, n2, n3, n4, n5, n6, n7, n8, n9,
		f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
		esc, tab, caps_lock, l_shift, l_ctrl, l_meta, l_alt, space, r_alt, r_meta, r_ctrl, r_shift, enter, backspace,
		print_screen, scroll_lock, pause, insert, home, page_up, del, end, page_down, up, down, left, right,
		backtick, hyphen, equals, open_bracket, close_bracket, backslash, semicolon, quote, comma, period, slash,
		num_lock, kp_slash, kp_asterisk, kp_minus, kp_plus, kp_enter, kp_dot, kp0, kp1, kp2, kp3, kp4, kp5, kp6, kp7, kp8, kp9
	};

	struct mouse_state {
		int x, y;
		bool left, right, middle;
	};

	class input_backend {
	protected:
		bool bad;
	public:
		input_backend();
		virtual ~input_backend() = default;

		virtual void update() = 0;
		virtual bool is_key_down(key k) const = 0;
		virtual mouse_state get_mouse_state() const = 0;
		virtual bool is_bad() const;

		static key to_key(char c);
	};
}
