#pragma once

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
			/* device owner */
			device_context const& dev;
			/* kernel framebuffer handle */
			uint32_t handle;

			/* kernel framebuffer id */
			uint32_t fb_id;
			/* mmap-ped memory pointer */
			uint32_t* mmio_ptr;
			/* mmap-ped memory size */
			size_t size;
			/* framebuffer pitch */
			uint32_t pitch;

		public:
			/* constructor */
			framebuffer(device_context const& d, uint32_t width, uint32_t height,
				bool *const success = nullptr);
			/* destructor */
			~framebuffer();

			/* blit */
			void copy_from(uint32_t const* src, size_t pixel_count) const;

			/* page flip */
			bool flip_onto(crtc const& c) const;

			/* apply contfig to CRTC */
			bool apply_config(crtc &c, std::uint32_t const connector_id,
				mode const& m) const;

			friend connector;
			friend backend;
		};
	}
}
