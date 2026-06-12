#pragma once

#include <drm/drm.h>
#include "rendering/rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		class mode : public rendering::rendering_mode {
			/* kernel drm_mode_modeinfo struct */
			struct drm_mode_modeinfo kernel_mode;

		public:

			/* constructor */
			mode(struct drm_mode_modeinfo const& info);
			/* destructor override with default */
			~mode() override = default;

			/* CRTC creation request for ioctl */
			struct drm_mode_crtc create_crtc_req(std::uint32_t const crtc_id,
				std::uint32_t const fb_id, std::uint32_t const *const conn_id) const;

			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"x_res">) const override;
			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"y_res">) const override;
			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"refresh_hz">) const override;
			/* componentized-like interface override */
			bool operator()(util::component_tag<"has_vsync">) const override;
		};
	}
}
