#include "rendering-mode.h"

namespace rendering {
	namespace drm_kms {
		drm_rendering_mode::drm_rendering_mode(const struct drm_mode_modeinfo& info)
			: kernel_mode(info) {
			this->x_res = info.hdisplay;
			this->y_res = info.vdisplay;
			this->refresh_hz = info.vrefresh;
			this->has_vsync = true;
		}
	}
}
