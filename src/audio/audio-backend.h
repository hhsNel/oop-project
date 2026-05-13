#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace audio {

	struct audio_format {
		unsigned int sample_rate;
		unsigned int channels;
		unsigned int bits_per_sample;
	};
	
	class audio_backend {
	protected:
		bool bad;

	public:
		audio_backend();
		virtual ~audio_backend() = default;

		// Otwiera urzadzenie audio z podanym formatem
		virtual bool open(audio_format const& fmt) = 0;

		// Zamyka urzadzenie audio
		virtual void close() = 0;
		virtual long write(void const* data, unsigned long frames) = 0;
		// Czeka az bufor sie oprozni (odtworzy caly dzwiek)
		virtual void drain() = 0;

		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual audio_format current_format() const = 0;
		virtual bool is_bad() const;
	};
}
