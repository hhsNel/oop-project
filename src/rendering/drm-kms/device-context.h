#pragma once

#include <string>

namespace rendering {
	namespace drm_kms {
		class device_context {
			/* dri file descriptor */
			int fd;

		public:

			/* constructor */
			explicit device_context(std::string const& path);
			/* destructor */
			~device_context();

			/* error checking */
			bool is_valid() const;
			/* sys/ioctl.h ::ioctl wrapper */
			int ioctl(unsigned long request, void* arg) const;
			/* sys/mman.h ::mmap wrapper */
			void *mmap(void *addr, size_t len, int prot, int flags, off_t off) const;
		};
	}
}
