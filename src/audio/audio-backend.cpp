#include "audio-backend.h"

namespace audio {
	audio_backend::audio_backend() : bad(false) {}

	bool audio_backend::is_bad() const { return bad; }
}
