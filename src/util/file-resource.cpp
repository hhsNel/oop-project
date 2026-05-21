#include "file-resource.h"

namespace util {
	void const* file_resource::operator()(component_tag<"beginning">) const {
		return file_data.data();
	}

	void const* file_resource::operator()(component_tag<"end">) const {
		return file_data.data() + file_data.size();
	}

	std::size_t file_resource::operator()(component_tag<"size">) const {
		return file_data.size();
	}

	file_resource::file_resource(std::vector<std::byte> data) :
		resource(),
		file_data(std::move(data))
	{}
}
