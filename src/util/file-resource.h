#pragma once
#define FILE_RESOURCE_H

#include <vector>
#include <cstddef>
#include "resource.h"

namespace util {
    class file_resource : public resource {
		std::vector<std::byte> file_data;

    public:
        explicit file_resource(std::vector<std::byte> data);
		~file_resource() override = default;
    };
}

