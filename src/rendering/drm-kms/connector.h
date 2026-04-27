#pragma once
#define DRM_KMS_CONNECTOR_H

#include <vector>
#include <memory>
#include <cstdint>
#include "device-context.h"
#include "rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		class connector {
		protected:
			const device_context& dev;
			bool bad;

		public:
			uint32_t connector_id;
			uint32_t encoder_id;

			connector(const device_context& d, uint32_t id);
			std::vector<std::unique_ptr<drm_rendering_mode const>> probe_modes();
			bool is_bad() const;
		};
	}
}
