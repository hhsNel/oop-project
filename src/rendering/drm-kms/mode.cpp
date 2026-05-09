#include "mode.h"

namespace rendering {
	namespace drm_kms {
		mode::mode(const struct drm_mode_modeinfo& info)
			: kernel_mode(info) {
			this->x_res = info.hdisplay;
			this->y_res = info.vdisplay;
			this->refresh_hz = info.vrefresh;
			this->has_vsync = true;
		}

		struct drm_mode_crtc mode::create_crtc_req(std::uint32_t const crtc_id, std::uint32_t const fb_id, std::uint32_t const *const conn_id) const {
			struct drm_mode_crtc crtc_req = {};

			crtc_req.crtc_id = crtc_id;
			crtc_req.fb_id = fb_id;
			crtc_req.set_connectors_ptr = reinterpret_cast<std::uint64_t>(conn_id);
			crtc_req.count_connectors = 1;
			crtc_req.mode = kernel_mode;
			crtc_req.mode_valid = 1;

			return crtc_req;
		}
	}
}
