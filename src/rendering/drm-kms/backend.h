#pragma once
#define DRM_KMS_BACKEND_H

#include <memory>
#include <vector>
#include <drm/drm.h>

#include "rendering/rendering-backend.h"
#include "device-context.h"
#include "framebuffer.h"
#include "connector.h"
#include "crtc.h"

namespace rendering {
	namespace drm_kms {
		class backend : public rendering::rendering_backend {
		protected:
			std::unique_ptr<device_context> dev;
			std::unique_ptr<connector> active_connector;
			std::unique_ptr<crtc> active_crtc;
			bool is_bad;

			std::unique_ptr<framebuffer> buffers[2];
			int front_buffer_index;
			std::vector<uint32_t> shadow;

			std::unique_ptr<mode const> current_mode;

			uint32_t original_fb_id = 0;
			uint32_t original_connector_id = 0;
			struct drm_mode_modeinfo original_mode = {};
			bool has_original_state = false;

			std::vector<std::unique_ptr<rendering_mode const>> obtain_modes() override;
			unsigned int obtain_width() override;
			unsigned int obtain_height() override;
			unsigned int obtain_pitch() override;
			std::uint32_t* obtain_mmio() override;

		public:
			backend();
			~backend() override;

			bool bad() const override;
			void push_mode(std::unique_ptr<rendering_mode const> pushed_mode) override;
			void wait_for_vsync() override;
			void flush() override;
		};
	}
}