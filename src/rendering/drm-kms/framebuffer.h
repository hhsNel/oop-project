#pragma once
#define DRM_KMS_FRAMEBUFFER_H

#include <cstdint>
#include <cstddef>
#include "device-context.h"
#include "mode.h"

namespace rendering {
	namespace drm_kms {
		class crtc;
		class connector;
		class backend;

		class framebuffer {
			const device_context& dev;
			uint32_t handle;

			uint32_t fb_id;
			uint32_t* mmio_ptr;
			size_t size;
			uint32_t pitch;

		public:
			framebuffer(const device_context& d, uint32_t width, uint32_t height, bool* const success = nullptr);
			~framebuffer();

			void copy_from(const uint32_t* src, size_t pixel_count) const;

			bool flip_onto(const crtc& c) const;

			bool apply_config(crtc &c, std::uint32_t const connector_id, const mode& m) const;

			friend connector;
			friend backend;
		};
	}
}
