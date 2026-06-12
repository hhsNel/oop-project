#pragma once
#define FILE_RESOURCE_H

#include <vector>
#include "resource.h"

namespace util {
	class file_resource : public resource {
		/* mapped file */
		std::vector<std::byte> file_data;

	public:
		/* constructor */
		explicit file_resource(std::vector<std::byte> data);
		/* destructor override with default */
		~file_resource() override = default;

		/* componentized-like API */
		void const* operator()(component_tag<"beginning">) const override;
		/* componentized-like API */
		void const* operator()(component_tag<"end">) const override;
		/* componentized-like API */
		std::size_t operator()(component_tag<"size">) const override;
	};
}
