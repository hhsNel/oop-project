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

		/* virtual aggregator */
		virtual bool bad() const = 0;
		/* componentized-like API for mode acquisition */
		virtual std::vector<std::unique_ptr<rendering_mode const>>
			operator()(util::component_tag<"modes">) = 0;
		/* mode setting */
		virtual void push_mode(std::unique_ptr<rendering_mode const> mode) = 0;
		/* componentized-like API */
		virtual unsigned int operator()(util::component_tag<"width">) = 0;
		/* componentized-like API */
		virtual unsigned int operator()(util::component_tag<"height">) = 0;
		/* componentized-like API */
		virtual unsigned int operator()(util::component_tag<"pitch">) = 0;
		/* componentized-like API */
		virtual std::uint32_t *operator()(util::component_tag<"mmio">) = 0;
		/* wait for hardware vsync, if current mode supports it */
		virtual void wait_for_vsync() = 0;
		/* flush to display */
		virtual void flush() = 0;

		/* virtual destructor */
		virtual ~rendering_backend() = default;
	};
}

