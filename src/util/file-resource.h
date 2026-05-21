#pragma once
#define FILE_RESOURCE_H

#include <vector>
#include "resource.h"

namespace util {
	class file_resource : public resource {
		std::vector<std::byte> file_data;

	public:
		explicit file_resource(std::vector<std::byte> data);
		~file_resource() override = default;

		void const* operator()(component_tag<"beginning">) const override;
		void const* operator()(component_tag<"end">) const override;
		std::size_t operator()(component_tag<"size">) const override;
	};
}
