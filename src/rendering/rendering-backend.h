#pragma once

#include <vector>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "util/componentized.h"
#include "rendering-mode.h"

namespace rendering {
	class rendering_backend {
	public:

		virtual bool bad() const = 0;
		virtual std::vector<std::unique_ptr<rendering_mode const>> operator()(util::component_tag<"modes">) = 0;
		virtual void push_mode(std::unique_ptr<rendering_mode const> mode) = 0;
		virtual unsigned int operator()(util::component_tag<"width">) = 0;
		virtual unsigned int operator()(util::component_tag<"height">) = 0;
		virtual unsigned int operator()(util::component_tag<"pitch">) = 0;
		virtual std::uint32_t *operator()(util::component_tag<"mmio">) = 0;
		virtual void wait_for_vsync() = 0;
		virtual void flush() = 0;

		virtual ~rendering_backend() = default;
	};
}

