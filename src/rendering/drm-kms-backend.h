#pragma once
#define DRM_KMS_BACKEND_H

#include <memory>
#include <vector>

#include "rendering-backend.h"
#include "drm-kms-device-context.h"
#include "drm-kms-framebuffer.h"
#include "drm-kms-connector.h"
#include "drm-kms-crtc.h"

namespace rendering {
	namespace drm_kms {
		class backend : public rendering::rendering_backend {
		protected:
			std::unique_ptr<device_context> dev;
			std::unique_ptr<connector> active_connector;
			std::unique_ptr<crtc> active_crtc;
			
			std::unique_ptr<framebuffer> buffers[2];
			int front_buffer_index;
			
			std::unique_ptr<drm_rendering_mode const> current_mode;

		public:
			backend();
			~backend() override;

			std::vector<std::unique_ptr<rendering_mode const>> get_modes() override;
			void set_mode(std::unique_ptr<rendering_mode const> mode) override;
			bool is_bad() const override;
			unsigned int get_width() override;
			unsigned int get_height() override;
			unsigned int get_pitch() override;
			std::uint32_t *get_mmio() override;
			void wait_for_vsync() override;
			void flush() override;
		};
	}
}
