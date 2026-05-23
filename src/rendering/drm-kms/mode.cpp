#include "mode.h"

namespace rendering {
	namespace drm_kms {
		mode::mode(const struct drm_mode_modeinfo& info)
			: kernel_mode(info) {}

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

		unsigned int mode::operator()(util::component_tag<"x_res">) const {
			return kernel_mode.hdisplay;
		}

		unsigned int mode::operator()(util::component_tag<"y_res">) const {
			return kernel_mode.vdisplay;
		}

		unsigned int mode::operator()(util::component_tag<"refresh_hz">) const {
			return kernel_mode.vrefresh;
		}

		bool mode::operator()(util::component_tag<"has_vsync">) const {
			return kernel_mode.flags & (DRM_MODE_FLAG_PVSYNC | DRM_MODE_FLAG_NVSYNC);
		}
	}
}
