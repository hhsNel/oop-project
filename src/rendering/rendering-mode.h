#pragma once
#define RENDERING_MODE_H

#include "util/componentized.h"

namespace rendering {
	class rendering_mode : public util::componentized<rendering_mode> {
	protected:
		[[=util::component_field{}]] unsigned int x_res;
		[[=util::component_field{}]] unsigned int y_res;
		[[=util::component_field{}]] unsigned int refresh_hz;
		[[=util::component_field{}]] bool has_vsync;
	public:
		virtual ~rendering_mode() = default;

		friend util::componentized<rendering_mode>;
	};
}

