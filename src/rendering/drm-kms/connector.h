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
			device_context const& dev;
			uint32_t connector_id;
			uint32_t encoder_id;

		public:
			connector(device_context const& d, uint32_t id, bool* const success = nullptr);

			std::vector<std::unique_ptr<mode const>> probe_modes();

			bool apply_config(crtc& c, framebuffer const& fb, mode const& m) const;
		};
	}
}
