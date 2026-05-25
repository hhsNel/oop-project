#pragma once
#define AUDIO_MIXER_H

#include "audio-backend.h"
#include "assets/audio-clip.h"
#include <vector>

namespace audio {

	class audio_mixer {
	private:
		struct playing_sound {
			assets::audio_clip const* clip;
			unsigned long current_frame;
		};

		audio_backend &target;
		audio_format mix_format;
		std::vector<playing_sound> active_sounds;

	public:
		explicit audio_mixer(audio_backend &tgt, audio_format const& fmt);

		void play(assets::audio_clip const& clip);

		void stop_all();
		
		void step(unsigned long frames_to_mix);

		bool is_silent() const;
	};
}