#include "drm-kms-connector.h"
#include <drm/drm.h>

namespace rendering {
	namespace drm_kms {
		connector::connector(const device_context& d, uint32_t id)
			: dev(d), bad(false), connector_id(id), encoder_id(0) {
			
			struct drm_mode_get_connector conn_req = {};
			conn_req.connector_id = connector_id;
			
			if (dev.ioctl(DRM_IOCTL_MODE_GETCONNECTOR, &conn_req) < 0) {
				bad = true;
				return;
			}

			encoder_id = conn_req.encoder_id;
		}

		bool connector::is_bad() const {
			return bad;
		}

		std::vector<std::unique_ptr<drm_rendering_mode const>> connector::probe_modes() {
			std::vector<std::unique_ptr<drm_rendering_mode const>> modes;
			
			if (is_bad()) return modes;

			struct drm_mode_get_connector conn_req = {};
			conn_req.connector_id = connector_id;

			if (dev.ioctl(DRM_IOCTL_MODE_GETCONNECTOR, &conn_req) < 0) {
				return modes;
			}

			if (conn_req.count_modes == 0) return modes;

			std::vector<struct drm_mode_modeinfo> raw_modes(conn_req.count_modes);
			conn_req.modes_ptr = reinterpret_cast<uint64_t>(raw_modes.data());

			if (dev.ioctl(DRM_IOCTL_MODE_GETCONNECTOR, &conn_req) == 0) {
				for (const auto& raw_mode : raw_modes) {
					modes.push_back(std::make_unique<drm_rendering_mode>(raw_mode));
				}
			}

			return modes;
		}
	}
}