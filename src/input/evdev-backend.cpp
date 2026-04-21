#include "evdev-backend.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>

namespace input {
	namespace evdev {
		backend::backend() :
			mouse({0, 0, false, false, false}),
			last_abs_x(-1),
			last_abs_y(-1) {
			probe_devices();
			if (devices.empty()) bad = true;
		}

		backend::~backend() {
			for (auto& dev : devices) {
				ioctl(dev.fd, EVIOCGRAB, 0);
				close(dev.fd);
			}
		}

		void backend::probe_devices() {
			for (int i = 0; i < 32; ++i) {
				std::string path = "/dev/input/event" + std::to_string(i);
				probe_device(path);
			}
			for (int i = 0; i < 32; ++i) {
				std::string path = "/dev/input/mouse" + std::to_string(i);
				probe_device(path);
			}
			probe_device("/dev/input/mice");
		}

		void backend::probe_device(std::string const dev_name) {
			int fd = open(dev_name.c_str(), O_RDONLY | O_NONBLOCK);
			if (fd < 0) return;

			unsigned long key_bitmask[(KEY_CNT + (sizeof(long) * 8) - 1) / (sizeof(long) * 8)] = {0};
			unsigned long rel_bitmask[(REL_CNT + (sizeof(long) * 8) - 1) / (sizeof(long) * 8)] = {0};
			unsigned long abs_bitmask[(ABS_CNT + (sizeof(long) * 8) - 1) / (sizeof(long) * 8)] = {0};

			bool has_keys = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(key_bitmask)), key_bitmask) >= 0;
			bool has_rel  = ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_bitmask)), rel_bitmask) >= 0;
			bool has_abs  = ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_bitmask)), abs_bitmask) >= 0;

			if (has_keys || has_rel || has_abs) {
				bool is_kb = has_keys && (key_bitmask[KEY_ENTER / (sizeof(long) * 8)] & (1UL << (KEY_ENTER % (sizeof(long) * 8))));

				bool has_left_click = has_keys && (key_bitmask[BTN_LEFT / (sizeof(long) * 8)] & (1UL << (BTN_LEFT % (sizeof(long) * 8))));
				bool has_x_axis = has_rel && (rel_bitmask[REL_X / (sizeof(long) * 8)] & (1UL << (REL_X % (sizeof(long) * 8))));
				bool has_abs_x = has_abs && (abs_bitmask[ABS_X / (sizeof(long) * 8)] & (1UL << (ABS_X % (sizeof(long) * 8))));
				
				bool is_ms = has_left_click || has_x_axis || has_abs_x;

				if (is_kb || is_ms) {
					ioctl(fd, EVIOCGRAB, 1);
					devices.push_back({fd, is_kb, is_ms});
					return;
				}
			}
			close(fd);
		}

		uint16_t backend::map_to_native(key k) const {
			switch (k) {

				case key::a: return KEY_A; case key::b: return KEY_B; case key::c: return KEY_C;
				case key::d: return KEY_D; case key::e: return KEY_E; case key::f: return KEY_F;
				case key::g: return KEY_G; case key::h: return KEY_H; case key::i: return KEY_I;
				case key::j: return KEY_J; case key::k: return KEY_K; case key::l: return KEY_L;
				case key::m: return KEY_M; case key::n: return KEY_N; case key::o: return KEY_O;
				case key::p: return KEY_P; case key::q: return KEY_Q; case key::r: return KEY_R;
				case key::s: return KEY_S; case key::t: return KEY_T; case key::u: return KEY_U;
				case key::v: return KEY_V; case key::w: return KEY_W; case key::x: return KEY_X;
				case key::y: return KEY_Y; case key::z: return KEY_Z;

				case key::n0: return KEY_0; case key::n1: return KEY_1; case key::n2: return KEY_2;
				case key::n3: return KEY_3; case key::n4: return KEY_4; case key::n5: return KEY_5;
				case key::n6: return KEY_6; case key::n7: return KEY_7; case key::n8: return KEY_8;
				case key::n9: return KEY_9;

				case key::f1: return KEY_F1;   case key::f2: return KEY_F2;   case key::f3: return KEY_F3;
				case key::f4: return KEY_F4;   case key::f5: return KEY_F5;   case key::f6: return KEY_F6;
				case key::f7: return KEY_F7;   case key::f8: return KEY_F8;   case key::f9: return KEY_F9;
				case key::f10: return KEY_F10; case key::f11: return KEY_F11; case key::f12: return KEY_F12;

				case key::esc: return KEY_ESC; case key::tab: return KEY_TAB; case key::caps_lock: return KEY_CAPSLOCK;
				case key::l_shift: return KEY_LEFTSHIFT; case key::l_ctrl: return KEY_LEFTCTRL;
				case key::l_meta: return KEY_LEFTMETA;   case key::l_alt: return KEY_LEFTALT;
				case key::space: return KEY_SPACE;       case key::r_alt: return KEY_RIGHTALT;
				case key::r_meta: return KEY_RIGHTMETA;  case key::r_ctrl: return KEY_RIGHTCTRL;
				case key::r_shift: return KEY_RIGHTSHIFT; case key::enter: return KEY_ENTER;
				case key::backspace: return KEY_BACKSPACE;

				case key::print_screen: return KEY_PRINT; case key::scroll_lock: return KEY_SCROLLLOCK;
				case key::pause: return KEY_PAUSE;       case key::insert: return KEY_INSERT;
				case key::home: return KEY_HOME;         case key::page_up: return KEY_PAGEUP;
				case key::del: return KEY_DELETE;        case key::end: return KEY_END;
				case key::page_down: return KEY_PAGEDOWN;
				case key::up: return KEY_UP;             case key::down: return KEY_DOWN;
				case key::left: return KEY_LEFT;         case key::right: return KEY_RIGHT;

				case key::backtick: return KEY_GRAVE;    case key::hyphen: return KEY_MINUS;
				case key::equals: return KEY_EQUAL;      case key::open_bracket: return KEY_LEFTBRACE;
				case key::close_bracket: return KEY_RIGHTBRACE; case key::backslash: return KEY_BACKSLASH;
				case key::semicolon: return KEY_SEMICOLON; case key::quote: return KEY_APOSTROPHE;
				case key::comma: return KEY_COMMA;       case key::period: return KEY_DOT;
				case key::slash: return KEY_SLASH;

				case key::num_lock: return KEY_NUMLOCK;  case key::kp_slash: return KEY_KPSLASH;
				case key::kp_asterisk: return KEY_KPASTERISK; case key::kp_minus: return KEY_KPMINUS;
				case key::kp_plus: return KEY_KPPLUS;    case key::kp_enter: return KEY_KPENTER;
				case key::kp_dot: return KEY_KPDOT;
				case key::kp0: return KEY_KP0; case key::kp1: return KEY_KP1; case key::kp2: return KEY_KP2;
				case key::kp3: return KEY_KP3; case key::kp4: return KEY_KP4; case key::kp5: return KEY_KP5;
				case key::kp6: return KEY_KP6; case key::kp7: return KEY_KP7; case key::kp8: return KEY_KP8;
				case key::kp9: return KEY_KP9;
				default: return 0;
			}
		}

		void backend::update() {
			struct input_event ev;
			for (auto& dev : devices) {
				while (read(dev.fd, &ev, sizeof(ev)) > 0) {
					process_event(ev);
				}
			}
		}

		void backend::process_event(const struct input_event& ev) {
			if (ev.type == EV_KEY && ev.code < KEY_CNT) {
				key_map[ev.code] = (ev.value > 0);
				if (ev.code == BTN_LEFT)   mouse.left = (ev.value > 0);
				if (ev.code == BTN_RIGHT)  mouse.right = (ev.value > 0);
				if (ev.code == BTN_MIDDLE) mouse.middle = (ev.value > 0);

				if (ev.code == BTN_TOUCH && ev.value == 0) {
					last_abs_x = -1;
					last_abs_y = -1;
				}
			} else if (ev.type == EV_REL) {
				if (ev.code == REL_X) mouse.x += ev.value;
				if (ev.code == REL_Y) mouse.y += ev.value;
			} else if (ev.type == EV_ABS) {
				if (ev.code == ABS_X) {
					if (last_abs_x != -1) mouse.x += (ev.value - last_abs_x);
					last_abs_x = ev.value;
				}
				if (ev.code == ABS_Y) {
					if (last_abs_y != -1) mouse.y += (ev.value - last_abs_y);
					last_abs_y = ev.value;
				}
			}
		}

		bool backend::is_key_down(key k) const {
			uint16_t native = map_to_native(k);
			return native != 0 && key_map[native];
		}

		mouse_state backend::get_mouse_state() const { return mouse; }
	}
}
