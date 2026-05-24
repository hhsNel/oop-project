#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "util/componentized.h"

namespace audio {

	struct audio_format {
		unsigned int sample_rate;
		unsigned int channels;
		unsigned int bits_per_sample;
	};
	
	class audio_backend {
	public:
		virtual ~audio_backend() = default;

		virtual bool open(audio_format const& fmt) = 0;
		virtual void close() = 0;
		virtual long write(void const* data, unsigned long frames) = 0;
		virtual void drain() = 0;

		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual audio_format operator()(util::component_tag<"current_format">) const = 0;
		virtual bool bad() const = 0;
	};
}
