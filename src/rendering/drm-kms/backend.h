#pragma once

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
			/* opened /dev/dri device */
			std::unique_ptr<device_context> dev;
			/* currently active connector */
			std::unique_ptr<connector> active_connector;
			/* currently active crtc */
			std::unique_ptr<crtc> active_crtc;
			/* error checking */
			bool is_bad;

			/* double-buffering */
			std::unique_ptr<framebuffer> buffers[2];
			/* currently active buffers index */
			int front_buffer_index;
			/* shadow buffer */
			std::vector<uint32_t> shadow;

			/* currently chosen mode */
			std::unique_ptr<mode const> current_mode;

			/* framebuffer id for tty VT driver */
			uint32_t original_fb_id = 0;
			/* connector id for tty VT driver */
			uint32_t original_connector_id = 0;
			/* mode for tty VT driver */
			struct drm_mode_modeinfo original_mode = {};
			/* whether or not original VT state was preserved */
			bool has_original_state = false;

		public:
			/* constructor */
			backend();
			/* destructor override */
			~backend() override;

			/* backend + fd aggregate */
			bool bad() const override;
			/* componentized-like interface override */
			virtual std::vector<std::unique_ptr<rendering_mode const>>
				operator()(util::component_tag<"modes">) override;
			/* virtual method override */
			void push_mode(std::unique_ptr<rendering_mode const> pushed_mode) override;
			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"width">) override;
			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"height">) override;
			/* componentized-like interface override */
			unsigned int operator()(util::component_tag<"pitch">) override;
			/* componentized-like interface override */
			std::uint32_t *operator()(util::component_tag<"mmio">) override;
			/* virtual method override */
			void wait_for_vsync() override;
			/* virtual method override */
			void flush() override;
		};
	}
}
