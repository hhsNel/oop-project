#include "input-backend.h"

namespace input {
	input_backend::input_backend() : bad(false) {}

	bool input_backend::is_bad() const { return bad; }

	key input_backend::to_key(char c) {
		if (c >= 'a' && c <= 'z') return static_cast<key>(static_cast<int>(key::a) + (c - 'a'));
		if (c >= 'A' && c <= 'Z') return static_cast<key>(static_cast<int>(key::a) + (c - 'A'));
		if (c >= '0' && c <= '9') return static_cast<key>(static_cast<int>(key::n0) + (c - '0'));

		switch (c) {
			case ' ':  return key::space;
			case '\t': return key::tab;
			case '\n': return key::enter;
			case '`':  return key::backtick;
			case '-':  return key::hyphen;
			case '=':  return key::equals;
			case '[':  return key::open_bracket;
			case ']':  return key::close_bracket;
			case '\\': return key::backslash;
			case ';':  return key::semicolon;
			case '\'': return key::quote;
			case ',':  return key::comma;
			case '.':  return key::period;
			case '/':  return key::slash;
			default:   return key::unknown;
		}
	}
}