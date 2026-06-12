#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "device-context.h"
#include "mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc;
		class framebuffer;

		class connector {
			/* device owner */
			device_context const& dev;
			/* kernel connector id */
			uint32_t connector_id;
			/* kernel encoder id */
			uint32_t encoder_id;

		public:
			/* constructor */
			connector(device_context const& d, uint32_t id, bool* const success = nullptr);

			/* returns std::vector of available modes for a particular connector */
			std::vector<std::unique_ptr<mode const>> probe_modes();

			/* config for ioctl */
			bool apply_config(crtc& c, framebuffer const& fb, mode const& m) const;
		};
	}
}
