#include "audio-mixer.h"
#include <algorithm>
#include <iostream>

namespace audio {

	audio_mixer::audio_mixer(audio_backend &tgt, audio_format const& fmt) 
		: target(tgt), mix_format(fmt) {
		target.open(mix_format);
	}

	void audio_mixer::play(assets::audio_clip const& clip) {
		audio_format const clip_fmt = clip("format"_f);
		
		if (clip_fmt.sample_rate != mix_format.sample_rate || 
			clip_fmt.bits_per_sample != 16) { 
			std::cerr << "Warning: Clip format doesn't match mixer format!" << std::endl;
			return;
		}
		
		active_sounds.push_back({&clip, 0});
	}

	void audio_mixer::step(unsigned long frames_to_mix) {
		if (target.is_bad()) return;

		unsigned int const channels = mix_format.channels;
		unsigned long const total_samples = frames_to_mix * channels;

		std::vector<std::int32_t> mix_buffer(total_samples, 0);

		for (auto it = active_sounds.begin(); it != active_sounds.end(); ) {
			assets::audio_clip const* clip = it->clip;
			unsigned long const clip_frames = (*clip)("total_frames"_f);
			
			auto const* pcm_16 = reinterpret_cast<std::int16_t const*>((*clip)("pcm_data"_f).data());

			unsigned long frames_to_read = std::min(frames_to_mix, clip_frames - it->current_frame);
			unsigned long samples_to_read = frames_to_read * channels;
			unsigned long start_sample = it->current_frame * channels;

			for (unsigned long i = 0; i < samples_to_read; ++i) {
				mix_buffer[i] += pcm_16[start_sample + i];
			}

			it->current_frame += frames_to_read;

			if (it->current_frame >= clip_frames) {
				it = active_sounds.erase(it);
			} else {
				++it;
			}
		}

		std::vector<std::int16_t> final_buffer(total_samples);
		for (unsigned long i = 0; i < total_samples; ++i) {
			final_buffer[i] = static_cast<std::int16_t>(
				std::clamp(mix_buffer[i], -32768, 32767)
			);
		}

		target.write(final_buffer.data(), frames_to_mix);
	}

	bool audio_mixer::is_silent() const {
		return active_sounds.empty();
	}
}
