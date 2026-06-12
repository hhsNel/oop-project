#pragma once
#define AUDIO_MIXER_H

#include "audio-backend.h"
#include "assets/audio-clip.h"
#include <vector>

namespace audio {

	class audio_mixer {
	private:
		/* one of mixed sounds */
		struct playing_sound {
			assets::audio_clip const* clip;
			unsigned long current_frame;
		};

		/* backend ref */
		audio_backend &target;
		/* supported format */
		audio_format mix_format;
		/* all currently playing sounds */
		std::vector<playing_sound> active_sounds;

	public:
		/* constructor */
		explicit audio_mixer(audio_backend &tgt, audio_format const& fmt);

		/* queue a sound */
		void play(assets::audio_clip const& clip);

		/* stop all sounds */
		void stop_all();
		
		/* play audio */
		void step(unsigned long frames_to_mix);

		/* are there no sounds playing */
		bool is_silent() const;
	};
}
