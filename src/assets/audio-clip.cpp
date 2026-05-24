#include "audio-clip.h"
#include <iostream>

namespace assets {

	audio_clip::audio_clip(std::vector<std::uint8_t> const& data, audio::audio_format const& fmt) :
		pcm_data(std::move(data)),
		format(fmt) {
		std::uint32_t bytes_per_frame = (fmt.bits_per_sample / 8) * fmt.channels;
		total_frames = (bytes_per_frame > 0) ? (pcm_data.size() / bytes_per_frame) : 0;
	}

	audio_clip const audio_clip::load_from_bin(util::resource const &res) {
		if (res("size"_f) < 12) {
			std::cerr << "empty audio clip" << std::endl;
			return audio_clip({}, {0, 0, 0});
		}

		const std::uint32_t* header = reinterpret_cast<const std::uint32_t*>(res("beginning"_f));
		audio::audio_format fmt;
		fmt.sample_rate = header[0];
		fmt.channels = header[1];
		fmt.bits_per_sample = header[2];

		const std::uint8_t* byte_data = reinterpret_cast<const std::uint8_t*>(res("beginning"_f)) + 12;
		std::size_t data_size = res("size"_f) - 12;

		std::vector<std::uint8_t> pcm(byte_data, byte_data + data_size);

		return audio_clip(pcm, fmt);
	}
}
