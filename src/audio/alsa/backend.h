#pragma once

#include <alsa/asoundlib.h>
#include "audio/audio-backend.h"

namespace audio {
	namespace alsa {
		class backend : public audio::audio_backend {
		private:
			bool is_bad;

			snd_pcm_t* pcm_handle;
			audio_format fmt;
			bool paused;

			snd_pcm_format_t to_alsa_format(unsigned int bits) const;

			bool configure_hw_params();

		public:
			backend();
			~backend() override;

			bool open(audio_format const& format) override;
			void close() override;
			long write(void const* data, unsigned long frames) override;
			void drain() override;
			void pause() override;
			void resume() override;
			audio_format operator()(util::component_tag<"current_format">) const override;
			bool bad() const override;
		};
	}
}
