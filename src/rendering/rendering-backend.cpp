#include "rendering-backend.h"

namespace rendering {
	rendering_backend::rendering_backend() : bad(false) {}

	bool rendering_backend::is_bad() const {
		return bad;
	}
}