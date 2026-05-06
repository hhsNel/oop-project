#include "file-resource.h"

util::file_resource::file_resource(std::vector<std::byte> data) :
	resource(nullptr, nullptr),
	file_data(std::move(data))
{
	begin = file_data.data();
	end = file_data.data() + file_data.size();
	size = file_data.size();
}

