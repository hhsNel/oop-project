#pragma once

#include "math/vec2.h"
#include "assets/ids.h"
#include "util/componentized.h"

namespace rendering {
	class software_renderer;
	class vissprite;

	class sprite : public util::componentized<sprite> {
	protected:

		/* sprite position */
		[[=util::ref_component_field{}]] math::vec2 pos;
		/* sprite height */
		[[=util::ref_component_field{}]] float z_pos;
		/* sprite angle */
		[[=util::ref_component_field{}]] float angle;
		/* sprite texture */
		assets::texture_id tex_id;
		/* texture scaling factor */
		float inherent_scale;
		/* flash time */
		float hit_flash = 0.0f;

	public:

		/* constructor */
		sprite(math::vec2 const p, float const z,
			assets::texture_id const tex, float const is);
		//sprite();
		/* virtual destructor */
		virtual ~sprite() = default;

		friend util::componentized<sprite>;
		friend software_renderer;
		friend vissprite;
	};
}

