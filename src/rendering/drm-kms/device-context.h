#pragma once
#define DRM_KMS_DEVICE_CONTEXT_H

#include <string>

namespace rendering {
	namespace drm_kms {
		class device_context {
		public:
			int fd;

			explicit device_context(const std::string& path);
			~device_context();

			bool is_valid() const;
			int ioctl(unsigned long request, void* arg) const;
		};
	}
}
