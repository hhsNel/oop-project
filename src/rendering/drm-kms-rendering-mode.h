#pragma once
#define DRM_KMS_RENDERING_MODE_H

#include <drm/drm.h>
#include "rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		class drm_rendering_mode : public rendering::rendering_mode {
		public:
			struct drm_mode_modeinfo kernel_mode;

			drm_rendering_mode(const struct drm_mode_modeinfo& info);
			~drm_rendering_mode() override = default;
		};
	}
}