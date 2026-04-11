#include "resource.h"

util::resource::resource() :
	begin(nullptr),
	end(nullptr),
	size(0)
{}

util::resource::resource(std::byte *begin_ptr, std::byte *end_ptr) :
	begin(begin_ptr),
	end(end_ptr),
	size(end_ptr - begin_ptr)
{}

