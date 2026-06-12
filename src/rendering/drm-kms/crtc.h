#pragma once

#include <cstdint>
#include "device-context.h"
#include "mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc {
			/* device owner */
			device_context const& dev;
			/* kernel CRTC id */
			std::uint32_t crtc_id;

		public:
			/* constructor */
			crtc(device_context const& d, uint32_t id);

			/* DRM_IOCTL_MODE_SETCRTC wrapper */
			bool set_config(uint32_t fb_id, uint32_t conn_id, mode const& m);

			/* DRM_IOCTL_MODE_PAGE_FLIP wrapper */
			bool page_flip(uint32_t fb_id) const;
		};
	}
}
