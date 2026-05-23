#pragma once
#define DRM_KMS_DEVICE_CONTEXT_H

#include <string>

namespace rendering {
	namespace drm_kms {
		class device_context {
			int fd;

		public:

			explicit device_context(std::string const& path);
			~device_context();

			bool is_valid() const;
			int ioctl(unsigned long request, void* arg) const;
			void *mmap(void *addr, size_t len, int prot, int flags, off_t off) const;
		};
	}
}
