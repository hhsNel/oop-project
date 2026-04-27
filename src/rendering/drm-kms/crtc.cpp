#include "crtc.h"
#include <drm/drm.h>

namespace rendering {
	namespace drm_kms {
		crtc::crtc(const device_context& d, uint32_t id)
			: dev(d), crtc_id(id) {}

		bool crtc::set_config(uint32_t fb_id, uint32_t conn_id, const drm_rendering_mode& mode) {
			struct drm_mode_crtc crtc_req = {};
			crtc_req.crtc_id = crtc_id;
			crtc_req.fb_id = fb_id;
			crtc_req.set_connectors_ptr = reinterpret_cast<uint64_t>(&conn_id);
			crtc_req.count_connectors = 1;
			crtc_req.mode = mode.kernel_mode;
			crtc_req.mode_valid = 1;

			return dev.ioctl(DRM_IOCTL_MODE_SETCRTC, &crtc_req) == 0;
		}
	}
}
