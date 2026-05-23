#pragma once
#define DRM_KMS_RENDERING_MODE_H

#include <drm/drm.h>
#include "rendering/rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		class mode : public rendering::rendering_mode {
			struct drm_mode_modeinfo kernel_mode;

		public:

			mode(const struct drm_mode_modeinfo& info);
			~mode() override = default;

			struct drm_mode_crtc create_crtc_req(std::uint32_t const crtc_id, std::uint32_t const fb_id, std::uint32_t const *const conn_id) const;

		unsigned int operator()(util::component_tag<"x_res">) const override;
		unsigned int operator()(util::component_tag<"y_res">) const override;
		unsigned int operator()(util::component_tag<"refresh_hz">) const override;
		bool operator()(util::component_tag<"has_vsync">) const override;
		};
	}
}
