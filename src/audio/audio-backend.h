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

	// Abstrakcyjny backend audio — konkretna implementacja w audio::alsa::backend
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

		// Zapisuje surowe probki PCM do urzadzenia
		// data — bufor z probkami, frames — liczba ramek (1 ramka = channels * sample)
		// Zwraca liczbe zapisanych ramek
		virtual long write(void const* data, unsigned long frames) = 0;

		// Wstrzymuje/wznawia odtwarzanie
		virtual void pause() = 0;
		virtual void resume() = 0;

		// Zwraca aktualny format audio
		virtual audio_format current_format() const = 0;

		// Sprawdza czy backend jest w stanie blednym
		virtual bool is_bad() const;
	};
}
