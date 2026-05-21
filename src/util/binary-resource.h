#pragma once
#define BINARY_RESOURCE_H

#include "resource.h"

namespace util {
	class binary_resource : public resource {
		void const* begin_ptr;
		void const* end_ptr;
		std::size_t byte_size;

	public:

		binary_resource(void const* begin, void const* end);
		~binary_resource() override = default;

		void const* operator()(component_tag<"beginning">) const override;
		void const* operator()(component_tag<"end">) const override;
		std::size_t operator()(component_tag<"size">) const override;
	};
}

