#pragma once
#define RENDERABLE_H

#include "graphics/texture-manager.h"
#include "math/segment.h"
#include "math/ray2.h"

namespace rendering {
	class renderable {
	public:
		math::segment seg;
		graphics::texture_manager::texture_id texture;
		math::fxpt z_begin;
		math::fxpt z_height;
		
		virtual bool intersects_with(math::ray2 const r) const;
		virtual ~renderable = default;
	};
}

