#pragma once
#define RENDERABLE_H

#include "graphics/texture-manager.h"
#include "math/segment.h"
#include "math/ray.h"

namespace rendering {
	class renderable {
	public:
		math::segment seg;
		graphics::texture_mgr::texture_id texture;
		
		virtual bool const intersects_with(math::ray const r) const;
		virtual ~renderable = default;
	};
}

