#include "crtc.h"
#include <drm/drm.h>

namespace rendering {
	namespace drm_kms {
		crtc::crtc(const device_context& d, uint32_t id)
			: dev(d), crtc_id(id) {}

		bool crtc::set_config(std::uint32_t fb_id, uint32_t conn_id, const mode& m) {
			struct drm_mode_crtc crtc_req = m.create_crtc_req(crtc_id, fb_id, conn_id);
			return dev.ioctl(DRM_IOCTL_MODE_SETCRTC, &crtc_req) == 0;
		}

		bool crtc::page_flip(uint32_t fb_id) const {
			struct drm_mode_crtc_page_flip flip_req = {};
			flip_req.crtc_id = crtc_id;
			flip_req.fb_id   = fb_id;
			return dev.ioctl(DRM_IOCTL_MODE_PAGE_FLIP, &flip_req) == 0;
		}
	}
}