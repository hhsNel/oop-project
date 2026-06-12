#pragma once
#define AUDIO_CLIP_H

#include <vector>
#include <cstdint>
#include "util/resource.h"
#include "util/componentized.h"
#include "audio/audio-backend.h"

namespace assets {
	class audio_clip : public util::componentized<audio_clip> {
		/* constructor */
		audio_clip(std::vector<std::uint8_t> const &data, audio::audio_format const &fmt);

		/* PCM data to play */
		[[=util::component_field{}]] std::vector<std::uint8_t> pcm_data;
		/* supported format */
		[[=util::component_field{}]] audio::audio_format format;
		/* length in frames */
		[[=util::component_field{}]] unsigned long total_frames;

	public:
		/* load from a binary */
		static audio_clip const load_from_bin(util::resource const &res);

		friend util::componentized<audio_clip>;
	};
}

