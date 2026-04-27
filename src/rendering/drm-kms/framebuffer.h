#pragma once
#define DRM_KMS_FRAMEBUFFER_H

#include <cstdint>
#include <cstddef>
#include "device-context.h"

namespace rendering {
	namespace drm_kms {
		class framebuffer {
		protected:
			const device_context& dev;
			uint32_t handle;
			bool bad;

		public:
			uint32_t fb_id;
			uint32_t* mmio_ptr;
			size_t size;
			uint32_t pitch;

			framebuffer(const device_context& d, uint32_t width, uint32_t height);
			~framebuffer();

			bool is_bad() const;
		};
	}
}
