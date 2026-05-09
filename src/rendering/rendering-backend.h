#pragma once
#define RENDERING_BACKEND_H

#include <vector>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "util/componentized.h"
#include "rendering-mode.h"

namespace rendering {
	class rendering_backend {
	protected:
		virtual std::vector<std::unique_ptr<rendering_mode const>> obtain_modes() = 0;
		virtual unsigned int obtain_width() = 0;
		virtual unsigned int obtain_height() = 0;
		virtual unsigned int obtain_pitch() = 0;
		virtual std::uint32_t *obtain_mmio() = 0;
	public:
		rendering_backend();

#define _STR(A) #A
#define STR(A) _STR(A)
#define _CONCAT(A,B) A##B
#define CONCAT(A,B) _CONCAT(A,B)
#define VIRTUAL_EXPOSE(TYPE,MEMBER) \
	template<util::fixed_string field> \
	requires (field.view() == STR(MEMBER)) \
	__attribute__((always_inline)) constexpr inline TYPE operator()(util::component_tag<field>) { \
		return CONCAT(obtain_,MEMBER)(); \
	}

		virtual bool bad() const = 0;
		VIRTUAL_EXPOSE(std::vector<std::unique_ptr<rendering_mode const>>, modes);
		virtual void push_mode(std::unique_ptr<rendering_mode const> mode) = 0;
		VIRTUAL_EXPOSE(unsigned int, width);
		VIRTUAL_EXPOSE(unsigned int, height);
		VIRTUAL_EXPOSE(unsigned int, pitch);
		VIRTUAL_EXPOSE(std::uint32_t *, mmio);
		virtual void wait_for_vsync() = 0;
		virtual void flush() = 0;

		virtual ~rendering_backend() = default;
	};
}

