#pragma once
#define RENDERING_MODE_H

namespace rendering {
	class rendering_mode {
	public:
		unsigned int x_res;
		unsigned int y_res;
		unsigned int refresh_hz;
		bool has_vsync;
	};
}

