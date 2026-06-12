#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "util/componentized.h"

namespace audio {

	/* supported format struct */
	struct audio_format {
		unsigned int sample_rate;
		unsigned int channels;
		unsigned int bits_per_sample;
	};
	
	class audio_backend {
	public:
		/* virtual destructor */
		virtual ~audio_backend() = default;

		/* set a new format */
		virtual bool open(audio_format const& fmt) = 0;
		/* stop */
		virtual void close() = 0;
		/* send pcm data */
		virtual long write(void const* data, unsigned long frames) = 0;
		/* drain the pcm buffer */
		virtual void drain() = 0;

		/* pause audio */
		virtual void pause() = 0;
		/* resume audio */
		virtual void resume() = 0;
		/* componentized-like api */
		virtual audio_format operator()(util::component_tag<"current_format">) const = 0;
		/* error checking */
		virtual bool bad() const = 0;
	};
}
