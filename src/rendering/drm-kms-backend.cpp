#include "drm-kms-backend.h"
#include <drm/drm.h>
#include <string>

namespace rendering {
	namespace drm_kms {
		backend::backend() : front_buffer_index(0), has_original_state(false) {
			bool pipeline_found = false;

			for (int i = 0; i < 64; ++i) {
				std::string device_path = "/dev/dri/card" + std::to_string(i);
				dev = std::make_unique<device_context>(device_path);

				if (!dev->is_valid()) {
					continue;
				}

				struct drm_mode_card_res res = {};
				if (dev->ioctl(DRM_IOCTL_MODE_GETRESOURCES, &res) < 0) {
					continue;
				}

				std::vector<uint32_t> connectors(res.count_connectors);
				std::vector<uint32_t> encoders(res.count_encoders);
				std::vector<uint32_t> crtcs(res.count_crtcs);

				res.connector_id_ptr = reinterpret_cast<uint64_t>(connectors.data());
				res.encoder_id_ptr = reinterpret_cast<uint64_t>(encoders.data());
				res.crtc_id_ptr = reinterpret_cast<uint64_t>(crtcs.data());

				if (dev->ioctl(DRM_IOCTL_MODE_GETRESOURCES, &res) < 0) {
					continue;
				}

				for (uint32_t conn_id : connectors) {
					struct drm_mode_get_connector conn_req = {};
					conn_req.connector_id = conn_id;
					dev->ioctl(DRM_IOCTL_MODE_GETCONNECTOR, &conn_req);

					if (conn_req.connection == 1 && conn_req.encoder_id != 0) {
						active_connector = std::make_unique<connector>(*dev, conn_id);

						struct drm_mode_get_encoder enc_req = {};
						enc_req.encoder_id = conn_req.encoder_id;
						dev->ioctl(DRM_IOCTL_MODE_GETENCODER, &enc_req);

						if (enc_req.crtc_id != 0) {
							active_crtc = std::make_unique<crtc>(*dev, enc_req.crtc_id);

							struct drm_mode_crtc old_crtc_req = {};
							old_crtc_req.crtc_id = enc_req.crtc_id;

							if (dev->ioctl(DRM_IOCTL_MODE_GETCRTC, &old_crtc_req) == 0) {
								original_fb_id = old_crtc_req.fb_id;
								original_connector_id = conn_id;
								original_mode = old_crtc_req.mode;
								has_original_state = true;
							}

							pipeline_found = true;
							break;
						}
					}
				}

				if (pipeline_found) {
					break;
				}
			}

			if (!pipeline_found) {
				bad = true;
			}
		}

		backend::~backend() {
			if (has_original_state && active_crtc && dev && dev->is_valid()) {
				drm_rendering_mode restore_mode(original_mode);
				active_crtc->set_config(original_fb_id, original_connector_id, restore_mode);
			}
		}

		std::vector<std::unique_ptr<rendering_mode const>> backend::get_modes() {
			if (is_bad()) return {};

			auto drm_modes = active_connector->probe_modes();
			std::vector<std::unique_ptr<rendering_mode const>> modes;
			for (auto& m : drm_modes) {
				modes.push_back(std::move(m));
			}
			return modes;
		}

		void backend::set_mode(std::unique_ptr<rendering_mode const> mode) {
			if (is_bad() || !mode) return;

			auto* drm_mode = static_cast<const drm_rendering_mode*>(mode.get());

			buffers[0] = std::make_unique<framebuffer>(*dev, drm_mode->x_res, drm_mode->y_res);
			buffers[1] = std::make_unique<framebuffer>(*dev, drm_mode->x_res, drm_mode->y_res);

			if (buffers[0]->is_bad() || buffers[1]->is_bad()) {
				bad = true;
				return;
			}

			front_buffer_index = 0;

			if (!active_crtc->set_config(buffers[front_buffer_index]->fb_id, active_connector->connector_id, *drm_mode)) {
				bad = true;
				return;
			}

			current_mode.reset(static_cast<const drm_rendering_mode*>(mode.release()));
		}

		bool backend::is_bad() const {
			return bad || !dev->is_valid();
		}

		unsigned int backend::get_width() {
			return current_mode ? current_mode->x_res : 0;
		}

		unsigned int backend::get_height() {
			return current_mode ? current_mode->y_res : 0;
		}

		unsigned int backend::get_pitch() {
			if (!buffers[0] || !buffers[1]) return 0;
			int back_index = 1 - front_buffer_index;
			return buffers[back_index]->pitch;
		}

		std::uint32_t* backend::get_mmio() {
			if (!buffers[0] || !buffers[1]) return nullptr;
			int back_index = 1 - front_buffer_index;
			return buffers[back_index]->mmio_ptr;
		}

		void backend::wait_for_vsync() {
			if (is_bad() || !active_crtc) return;

			union drm_wait_vblank vbl = {};
			vbl.request.type = (drm_vblank_seq_type)(_DRM_VBLANK_RELATIVE);
			vbl.request.sequence = 1;

			dev->ioctl(DRM_IOCTL_WAIT_VBLANK, &vbl);
		}

		void backend::flush() {
			if (is_bad() || !active_crtc || !buffers[0] || !buffers[1]) return;

			int back_index = 1 - front_buffer_index;

			struct drm_mode_crtc_page_flip flip_req = {};
			flip_req.crtc_id = active_crtc->crtc_id;
			flip_req.fb_id = buffers[back_index]->fb_id;

			if (dev->ioctl(DRM_IOCTL_MODE_PAGE_FLIP, &flip_req) == 0) {
				front_buffer_index = back_index;
			}
		}
	}
}
