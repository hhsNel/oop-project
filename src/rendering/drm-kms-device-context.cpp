#include "drm-kms-device-context.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <drm/drm.h>

namespace rendering {
	namespace drm_kms {
		device_context::device_context(const std::string& path) {
			fd = open(path.c_str(), O_RDWR | O_CLOEXEC);
			
			if (is_valid()) {
				ioctl(DRM_IOCTL_SET_MASTER, nullptr);
			}
		}

		device_context::~device_context() {
			if (is_valid()) {
				ioctl(DRM_IOCTL_DROP_MASTER, nullptr);
				close(fd);
			}
		}

		bool device_context::is_valid() const {
			return fd >= 0;
		}

		int device_context::ioctl(unsigned long request, void* arg) const {
			return ::ioctl(fd, request, arg);
		}
	}
}
