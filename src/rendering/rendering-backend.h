#pragma once
#define RENDERING_BACKEND_H

#include <vector>
#include <memory>
#include <cstddef>

#include "rendering-mode.h"

namespace rendering {
	class rendering_backend {
		bool is_bad;
	public:
		rendering_backend();

		virtual std::vector< std::shared_ptr<rendering_mode const> const > const get_modes() = 0;
		virtual void set_mode(std::shared_ptr<rendering_mode const> const mode) = 0;
		bool is_bad() const;
		virtual unsigned int const get_width() = 0;
		virtual unsigned int const get_height() = 0;
		virtual void wait_for_vsync() = 0;
		virtual void flush() = 0;
		virtual unsigned int const get_pitch() = 0;
		virtual std::byte *const get_mmio() = 0;

		virtual ~rendering_backend() = default;
	};
}

