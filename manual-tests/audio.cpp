#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

#include "audio/audio-backend.h"
#include "audio/alsa/backend.h"

// Generuje sinusoidalne probki
// freq — czestotliwosc w Hz, duration — czas w sekundach
std::vector<std::int16_t> generate_sine(
	unsigned int sample_rate,
	unsigned int channels,
	float freq,
	float duration)
{
	auto total_frames = static_cast<unsigned int>(sample_rate * duration);
	std::vector<std::int16_t> samples(total_frames * channels);

	// ~80% max int16
	constexpr float amplitude = 26000.0f;

	for (unsigned int i = 0; i < total_frames; ++i) {
		auto value = static_cast<std::int16_t>(
			amplitude * std::sin(2.0f * static_cast<float>(M_PI) * freq
				* static_cast<float>(i) / static_cast<float>(sample_rate))
		);
		for (unsigned int ch = 0; ch < channels; ++ch)
			samples[i * channels + ch] = value;
	}

	return samples;
}

int main() {
	std::cout << "=== audio backend manual test ===" << std::endl;

	// 1. Tworzenie backendu
	std::cout << "[1] tworzenie alsa backend... ";
	std::unique_ptr<audio::audio_backend> backend =
		std::make_unique<audio::alsa::backend>();

	if (backend->is_bad()) {
		std::cerr << "FAIL (backend w stanie blednym po konstrukcji)" << std::endl;
		return 1;
	}
	std::cout << "OK" << std::endl;

	// 2. Otwieranie urzadzenia
	audio::audio_format fmt{44100, 2, 16};
	std::cout << "[2] open(" << fmt.sample_rate << " Hz, "
	          << fmt.channels << " ch, "
	          << fmt.bits_per_sample << " bit)... ";

	if (!backend->open(fmt)) {
		std::cerr << "FAIL (nie udalo sie otworzyc urzadzenia)" << std::endl;
		std::cerr << "    (sprawdz czy ALSA jest dostepna i urzadzenie nie jest zajete)" << std::endl;
		return 1;
	}
	std::cout << "OK" << std::endl;

	// 3. Sprawdzenie wynegocjowanego formatu
	audio::audio_format actual = backend->current_format();
	std::cout << "[3] wynegocjowany format: "
	          << actual.sample_rate << " Hz, "
	          << actual.channels << " ch, "
	          << actual.bits_per_sample << " bit" << std::endl;

	// 4. Generowanie 5s sygnalu 440 Hz
	std::cout << "[4] generowanie sygnalu 440 Hz (5s)... ";
	auto samples = generate_sine(actual.sample_rate, actual.channels, 440.0f, 5.0f);
	unsigned long total_frames = samples.size() / actual.channels;
	std::cout << "OK (" << total_frames << " ramek)" << std::endl;

	// 5. Zapis calego bufora
	std::cout << "[5] write()... ";
	long written = backend->write(samples.data(), total_frames);
	if (written < 0) {
		std::cerr << "FAIL (write zwrocil " << written << ")" << std::endl;
		return 1;
	}
	std::cout << "OK (" << written << " ramek)" << std::endl;

	// 6. drain() — czeka az caly bufor zostanie odtworzony
	std::cout << "[6] drain() — czekam na odtworzenie... " << std::flush;
	backend->drain();
	std::cout << "OK" << std::endl;

	// 7. Test pause/resume
	std::cout << "[7] pause()... ";
	backend->pause();
	if (backend->is_bad()) {
		std::cerr << "FAIL" << std::endl;
		return 1;
	}
	std::cout << "OK" << std::endl;

	std::cout << "[8] resume()... ";
	backend->resume();
	if (backend->is_bad()) {
		std::cerr << "FAIL" << std::endl;
		return 1;
	}
	std::cout << "OK" << std::endl;

	// 9. Zamkniecie
	std::cout << "[9] close()... ";
	backend->close();
	std::cout << "OK" << std::endl;

	// 10. Stan po zamknieciu
	std::cout << "[10] is_bad() po close: "
	          << (backend->is_bad() ? "FAIL (bad=true)" : "OK (bad=false)")
	          << std::endl;

	std::cout << "\n=== wszystkie testy przeszly ===" << std::endl;
	return 0;
}
