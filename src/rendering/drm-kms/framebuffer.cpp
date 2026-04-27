#include "framebuffer.h"
#include <sys/mman.h>
#include <drm/drm.h>
#include <cstring>

namespace rendering {
	namespace drm_kms {
		framebuffer::framebuffer(const device_context& d, uint32_t width, uint32_t height)
			: dev(d), handle(0), bad(false), fb_id(0), mmio_ptr(nullptr), size(0), pitch(0) {
			
			struct drm_mode_create_dumb create_req = {};
			create_req.width = width;
			create_req.height = height;
			create_req.bpp = 32;

			if (dev.ioctl(DRM_IOCTL_MODE_CREATE_DUMB, &create_req) < 0) {
				bad = true;
				return;
			}

			handle = create_req.handle;
			pitch = create_req.pitch;
			size = create_req.size;

			struct drm_mode_fb_cmd fb_req = {};
			fb_req.width = width;
			fb_req.height = height;
			fb_req.pitch = pitch;
			fb_req.bpp = 32;
			fb_req.depth = 24;
			fb_req.handle = handle;

			if (dev.ioctl(DRM_IOCTL_MODE_ADDFB, &fb_req) < 0) {
				bad = true;
				return;
			}
			fb_id = fb_req.fb_id;

			struct drm_mode_map_dumb map_req = {};
			map_req.handle = handle;
			if (dev.ioctl(DRM_IOCTL_MODE_MAP_DUMB, &map_req) < 0) {
				bad = true;
				return;
			}

			mmio_ptr = static_cast<uint32_t*>(mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, dev.fd, map_req.offset));
			if (mmio_ptr == MAP_FAILED) {
				mmio_ptr = nullptr;
				bad = true;
				return;
			}

			/* maybe we don't want to clear the screen? idk */
			// std::memset(mmio_ptr, 0, size);
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

		bool framebuffer::is_bad() const {
			return bad;
		}
	}
}
