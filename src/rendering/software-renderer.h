#pragma once
#define SOFTWARE_RENDERER_H

#include <memory>

#include "rendering-backend.h"
#include "util/indexed-storage.h"
#include "renderable.h"
#include "math/fxpt.h"
#include "math/vec2.h"

namespace rendering {
	class software_renderer {
		rendering_backend *target;
	public:
		software_renderer();

		void set_target(rendering_backend *const target);
		void render(util::indexed_storage< std::shared_ptr<renderable> > const& targets, math::vec2 const camera_pos, math::fxpt const camera_angle);
	};
}

