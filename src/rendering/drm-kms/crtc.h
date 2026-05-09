#pragma once
#define DRM_KMS_CRTC_H

#include <cstdint>
#include "device-context.h"
#include "mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc {
			const device_context& dev;
			std::uint32_t crtc_id;

		public:
			crtc(const device_context& d, uint32_t id);

			// Apply a full modesetting config: bind a framebuffer, connector, and mode.
			bool set_config(uint32_t fb_id, uint32_t conn_id, const mode& m);

			// Issue a non-blocking page flip to the given framebuffer.
			// Called by framebuffer::flip_onto — crtc_id stays private.
			bool page_flip(uint32_t fb_id) const;
		};
	}
}