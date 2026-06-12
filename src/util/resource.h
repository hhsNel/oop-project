#pragma once
#define RESOURCE_H

#include <cstddef>
#include "util/componentized.h"

namespace util {
	class resource {
	public:
		/* virtual destructor */
		virtual ~resource() = default;

		/* componentized-like API */
		virtual void const* operator()(component_tag<"beginning">) const = 0;
		/* componentized-like API */
		virtual void const* operator()(component_tag<"end">) const = 0;
		/* componentized-like API */
		virtual std::size_t operator()(component_tag<"size">) const = 0;
	};
}
