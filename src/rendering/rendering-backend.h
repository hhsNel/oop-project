#pragma once
#define RENDERING_BACKEND_H

#include <vector>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "rendering-mode.h"

namespace rendering {
	class rendering_backend {
	protected:
		bool bad;
	public:
		rendering_backend();

		virtual std::vector<std::unique_ptr<rendering_mode const>> get_modes() = 0;
		virtual void set_mode(std::unique_ptr<rendering_mode const> mode) = 0;
		virtual bool is_bad() const;
		virtual unsigned int get_width() = 0;
		virtual unsigned int get_height() = 0;
		virtual void wait_for_vsync() = 0;
		virtual void flush() = 0;
		virtual unsigned int get_pitch() = 0;
		virtual std::uint32_t *get_mmio() = 0;

		virtual ~rendering_backend() = default;
	};
}

