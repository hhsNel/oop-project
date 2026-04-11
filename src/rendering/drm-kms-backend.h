#pragma once
#define DRM_KMS_BACKEND_H

#include <vector>

#include "rendering-backend.h"

namespace rendering {
	namespace drm_kms {
		class drm_kms_backend {
			bool is_bad;
		public:
			drm_kms_backend();

			std::vector<rendering_mode const> const get_modes() override;
			void set_mode(rendering_mode const mode) override;
			unsigned int const get_width() override;
			unsigned int const get_height() override;
			void wait_for_vsync() override;
			void flush() override;
			unsigned int const get_pitch() override;
			void *const get_mmio() override;
		};
	}
}

