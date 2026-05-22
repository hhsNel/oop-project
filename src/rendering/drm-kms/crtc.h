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

			bool set_config(uint32_t fb_id, uint32_t conn_id, const mode& m);

			bool page_flip(uint32_t fb_id) const;
		};
	}
}
