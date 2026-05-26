#pragma once

#include "util/componentized.h"

namespace rendering {
	class rendering_mode : public util::componentized<rendering_mode> {
	public:
		virtual ~rendering_mode() = default;

		virtual unsigned int operator()(util::component_tag<"x_res">) const = 0;
		virtual unsigned int operator()(util::component_tag<"y_res">) const = 0;
		virtual unsigned int operator()(util::component_tag<"refresh_hz">) const = 0;
		virtual bool operator()(util::component_tag<"has_vsync">) const = 0;

		friend util::componentized<rendering_mode>;
	};
}

