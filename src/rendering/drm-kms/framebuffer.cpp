#include "framebuffer.h"
#include "crtc.h"
#include <drm/drm.h>
#include <cstring>
#include <sys/mman.h>

namespace rendering {
	namespace drm_kms {
		framebuffer::framebuffer(const device_context& d, uint32_t width, uint32_t height, bool* const success)
			: dev(d), handle(0), fb_id(0), mmio_ptr(nullptr), size(0), pitch(0) {

			struct drm_mode_create_dumb create_req = {};
			create_req.width = width;
			create_req.height = height;
			create_req.bpp = 32;

			if (dev.ioctl(DRM_IOCTL_MODE_CREATE_DUMB, &create_req) < 0) {
				if (success) *success = false;
				return;
			}

			handle = create_req.handle;
			pitch  = create_req.pitch;
			size   = create_req.size;

			struct drm_mode_fb_cmd fb_req = {};
			fb_req.width  = width;
			fb_req.height = height;
			fb_req.pitch  = pitch;
			fb_req.bpp    = 32;
			fb_req.depth  = 24;
			fb_req.handle = handle;

			if (dev.ioctl(DRM_IOCTL_MODE_ADDFB, &fb_req) < 0) {
				if (success) *success = false;
				return;
			}
			fb_id = fb_req.fb_id;

			struct drm_mode_map_dumb map_req = {};
			map_req.handle = handle;
			if (dev.ioctl(DRM_IOCTL_MODE_MAP_DUMB, &map_req) < 0) {
				if (success) *success = false;
				return;
			}

			mmio_ptr = static_cast<uint32_t*>(dev.mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, map_req.offset));
			if (mmio_ptr == nullptr) {
				if (success) *success = false;
				return;
			}

			if (success) *success = true;
		}

		framebuffer::~framebuffer() {
			if (mmio_ptr != nullptr) {
				munmap(mmio_ptr, size);
			}
			if (fb_id != 0) {
				dev.ioctl(DRM_IOCTL_MODE_RMFB, &fb_id);
			}
			if (handle != 0) {
				struct drm_mode_destroy_dumb destroy_req = {};
				destroy_req.handle = handle;
				dev.ioctl(DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_req);
			}
		}

		void framebuffer::copy_from(const uint32_t* src, size_t pixel_count) const {
			std::memcpy(mmio_ptr, src, pixel_count * sizeof(uint32_t));
		}

		bool framebuffer::flip_onto(const crtc& c) const {
			return c.page_flip(fb_id);
		}

		bool framebuffer::apply_config(crtc &c, std::uint32_t const connector_id, const mode& m) const {
			return c.set_config(fb_id, connector_id, m);
		}
	}
}
