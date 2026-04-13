#include "resource.h"

util::resource::resource(std::byte const *const begin_ptr, std::byte const *const end_ptr) :
	begin(begin_ptr),
	end(end_ptr),
	size(end_ptr - begin_ptr)
{}

