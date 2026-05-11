#pragma once
#define RESOURCE_H

#include <cstddef>
#include "util/componentized.h"

namespace util {
	class resource {
	public:
		virtual ~resource() = default;

//		resource(const resource&)            = delete;
//		resource& operator=(const resource&) = delete;

		virtual void const* operator()(component_tag<"beginning">) const = 0;
		virtual void const* operator()(component_tag<"end">) const = 0;
		virtual std::size_t operator()(component_tag<"size">) const = 0;
	};
}
