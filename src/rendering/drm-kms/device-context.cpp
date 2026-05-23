#include "device-context.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <drm/drm.h>
#include <sys/mman.h>

namespace rendering {
	namespace drm_kms {
		device_context::device_context(std::string const& path) {
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

		void *device_context::mmap(void *addr, size_t len, int prot, int flags, off_t off) const {
			void *memory = ::mmap(addr, len, prot, flags, fd, off);

			if(memory == MAP_FAILED) {
				memory = nullptr;
			}

			return memory;
		}
	}
}
