#pragma once
#define BINARY_RESOURCE_H

#include "resource.h"

namespace util {
	class binary_resource : public resource {
		/* ptr to the beginning of baked binary date */
		void const* begin_ptr;
		/* ptr to the end of baked binary date */
		void const* end_ptr;
		/* size of baked binary date */
		std::size_t byte_size;

	public:

		/* constructor */
		binary_resource(void const* begin, void const* end);
		/* destructor */
		~binary_resource() override = default;

		/* componentized-like API */
		void const* operator()(component_tag<"beginning">) const override;
		/* componentized-like API */
		void const* operator()(component_tag<"end">) const override;
		/* componentized-like API */
		std::size_t operator()(component_tag<"size">) const override;
	};
}

