#pragma once

#include "math/vec2.h"
#include "assets/ids.h"
#include "util/componentized.h"

namespace rendering {
	class software_renderer;
	class vissprite;

	class sprite : public util::componentized<sprite> {
	protected:

		[[=util::ref_component_field{}]] math::vec2 pos;
		[[=util::ref_component_field{}]] float z_pos;
		[[=util::ref_component_field{}]] float angle;
		assets::texture_id tex_id;
		float inherent_scale;

	public:

		sprite(math::vec2 const p, float const z, assets::texture_id const tex, float const is);
		//sprite();
		virtual ~sprite() = default;

		friend util::componentized<sprite>;
		friend software_renderer;
		friend vissprite;
	};
}

