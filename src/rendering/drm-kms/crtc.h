#pragma once

#include <cstdint>
#include "device-context.h"
#include "mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc {
			device_context const& dev;
			std::uint32_t crtc_id;

		public:
			crtc(device_context const& d, uint32_t id);

			bool set_config(uint32_t fb_id, uint32_t conn_id, mode const& m);

			bool page_flip(uint32_t fb_id) const;
		};
	}
}
