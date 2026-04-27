#pragma once
#define DRM_KMS_CRTC_H

#include <cstdint>
#include "device-context.h"
#include "rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc {
		protected:
			const device_context& dev;

		public:
			uint32_t crtc_id;

			crtc(const device_context& d, uint32_t id);
			bool set_config(uint32_t fb_id, uint32_t conn_id, const drm_rendering_mode& mode);
		};
	}
}
